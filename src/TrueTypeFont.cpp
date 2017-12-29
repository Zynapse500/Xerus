//
// Created by Christofer Nolander on 2017-12-29.
//

#include <vector>
#include <BitmapFont.h>
#include "TrueTypeFont.h"

ft::FT_Library xr::TrueTypeFont::library;

xr::TrueTypeFont::TrueTypeFont() {
    initFreetype();
}

xr::TrueTypeFont::TrueTypeFont(const char *path, int size, bool flipVertically) :
    TrueTypeFont()
{
    face = loadFace(path);
    setFaceSize(face, size);

    std::vector<char> characters;
    for (char c = ' '; c <= '~'; ++c) { characters.push_back(c); }

    generateBitmapFont(face, characters, flipVertically);
}

void xr::TrueTypeFont::initFreetype() {
    static bool isInit = false;

    if (!isInit) {
        if (ft::FT_Error error = ft::FT_Init_FreeType(&library)) {
            throw std::runtime_error("Failed to init Freetype!");
        }
    }

    isInit = true;
}

ft::FT_Face xr::TrueTypeFont::loadFace(const char *path) {
    ft::FT_Face face;

    ft::FT_Error error = ft::FT_New_Face(library, path, 0, &face);

    if (error == ft::FT_Err_Unknown_File_Format) {
        throw std::runtime_error("The font file format it unsupported!");
    } else if (error) {
        throw std::runtime_error("Failed to read font file!");
    }

    return face;
}

void xr::TrueTypeFont::setFaceSize(ft::FT_Face &face, int size) {
    ft::FT_Error error = ft::FT_Set_Pixel_Sizes(face, 0, static_cast<ft::FT_UInt>(size));

    if (error) {
        throw std::runtime_error("Failed to set font size!");
    }
}

void
xr::TrueTypeFont::generateBitmapFont(ft::FT_Face &face, const std::vector<char> &characterCodes, bool flipVertically) {

    std::vector<Character> characters;
    std::vector<Image> images;

    for (auto &&charCode : characterCodes) {
        auto glyphIndex = ft::FT_Get_Char_Index(face, (ft::FT_ULong) charCode);

        ft::FT_Error error = ft::FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
        if (error) {
            continue;
        }

        ft::FT_GlyphSlot slot = face->glyph;

        if (slot->format != ft::FT_GLYPH_FORMAT_BITMAP) {
            error = ft::FT_Render_Glyph(slot, ft::FT_RENDER_MODE_NORMAL);

            if(error) {
                continue;
            }
        }


        int w = slot->bitmap.width;
        int h = slot->bitmap.rows;

        // Convert bitmap to RGBA
        std::vector<unsigned char> bitmap(static_cast<unsigned int>(w * h * 4));
        int bitmapSize = bitmap.size();
        for (int i = 0; i < bitmapSize; i+=4) {
            bitmap[i + 0] = 255;
            bitmap[i + 1] = 255;
            bitmap[i + 2] = 255;
            bitmap[i + 3] = slot->bitmap.buffer[i / 4];
        }

        // Create image
        images.emplace_back(bitmap, w, h);

        // Construct character
        Character character{};
        character.size.x = w;
        character.size.y = flipVertically ? h : -h;

        character.advance = slot->advance.x / 64.f;
        character.offset.x = slot->metrics.horiBearingX / 64.f;
        character.offset.y = (flipVertically ? -slot->metrics.horiBearingY : slot->metrics.horiBearingY) / 64.f;

        characters.push_back(character);
    }

    Image atlas;
    std::vector<ImageRegion> regions = stitchImages(atlas, images);

    Texture texture {atlas};

    for (int i = 0; i < regions.size(); ++i) {
        regions[i].y += regions[i].height;
        regions[i].height *= -1;

        characters[i].region = TextureRegion(regions[i], texture);
        this->registerCharacter(characterCodes[i], characters[i]);
    }
}

