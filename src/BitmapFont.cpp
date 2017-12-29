//
// Created by Christofer Nolander on 2017-12-29.
//

#include <iostream>
#include "stdafx.h"
#include "BitmapFont.h"


void xr::BitmapFont::registerCharacter(char c, const xr::BitmapFont::Character &character) {
    this->characters[c] = character;
}

void xr::BitmapFont::renderText(std::string text, glt::vec2f position, xr::RenderBatch *batch) const {
    for (char ch : text) {
        auto it = characters.find(ch);

        if (it != characters.end()) {
            const Character& character = characters[ch];
            batch->setTexture(character.region);

            batch->fillRect(position, character.size);

            position.x += character.size.x;
        } else {
            std::cerr << "Font missing character: " << ch << std::endl;
        }
    }
}
