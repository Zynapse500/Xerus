//
// Created by Christofer Nolander on 2017-12-29.
//

#pragma once

#include <string>

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

            bool operator<(const Character* other) const {
                return this->region.getTexture() < other->region.getTexture();
            }
        };


        // Registers a new character
        void registerCharacter(char c, const Character& character);

    public:

        void renderText(std::string text, glt::vec2f position, RenderBatch *batch) const;

    private:

        // Map of all characters
        std::map<char, Character> characters;

    };
}
