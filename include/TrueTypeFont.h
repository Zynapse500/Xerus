//
// Created by Christofer Nolander on 2017-12-29.
//

#pragma once


#include "BitmapFont.h"


// Include the font rendering library, freetype
namespace ft {
    #include <ft2build.h>
    #include FT_FREETYPE_H
}


namespace xr {
    class TrueTypeFont: public BitmapFont {

        // This font's face
        ft::FT_Face face;


    public:

        // Create a new font
        TrueTypeFont();


        // Load a new font from specified path
        TrueTypeFont(const char* path, int size, bool flipVertically = false);


    private:

        // Initializes the freetype library
        static void initFreetype();

        // Handle to the freetype library
        static ft::FT_Library library;

        // Loads a new freetype face from disk
        static ft::FT_Face loadFace(const char* path);

        // Sets the size of the face in pixels
        static void setFaceSize(ft::FT_Face& face, int size);


        // Generates a bitmap font from a face
        void generateBitmapFont(ft::FT_Face &face, const std::vector<char> &characterCodes, bool flipVertically = false);
    };
}
