//
// Created by Christofer Nolander on 2017-12-29.
//

#include "stdafx.h"

#include <iostream>
#include "BitmapFont.h"


void xr::BitmapFont::registerCharacter(char c, const xr::BitmapFont::Character &character) {
    this->characters[c] = character;
}

void xr::BitmapFont::renderText(std::string text, glt::vec2f position, xr::RenderBatch *batch) {
    for (char ch : text) {
        auto it = characters.find(ch);

        if (it != characters.end()) {
            const Character& character = it->second;
            batch->setTexture(character.region);

            batch->fillRect(position + character.offset, character.size);

            position.x += character.advance;
        } else {
            std::cerr << "Font missing character: " << ch << std::endl;
        }
    }
}
