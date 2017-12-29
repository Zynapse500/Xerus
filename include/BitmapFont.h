//
// Created by Christofer Nolander on 2017-12-29.
//

#pragma once

#include <string>
#include <map>

#include "RenderBatch.h"

namespace xr {
    class BitmapFont {


    protected:

        // Stores information about a character
        struct Character {
            // The texture of the character
            TextureRegion region;

            // Size of the character
            glt::vec2f size;

            // Offset from character origin
            glt::vec2f offset;

            // How far along should the next character be?
            float advance;

            bool operator<(const Character* other) const {
                return this->region.getTexture() < other->region.getTexture();
            }
        };


        // Registers a new character
        void registerCharacter(char c, const Character& character);

    public:

        void renderText(std::string text, glt::vec2f position, RenderBatch *batch);

    private:

        // Map of all characters
        std::map<char, Character> characters;

    };
}
