//
// Created by Anton on 16.08.2022.
//

#ifndef GRAPHICS_UIHANDLER_H
#define GRAPHICS_UIHANDLER_H

#include <iostream>
#include <map>
#include <ft2build.h>
#include <memory>
#include FT_FREETYPE_H
#include "../include/OpenGL/include/glm/glm.hpp"
#include "../Render/Shader.h"


struct Character
{
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

class UIHandler
{
private:
    static std::map<char, Character> Characters;
    static unsigned int VAO, VBO;

private:
    // render state
public:
    /* Restriction to create an instance of this class */
    UIHandler() = delete;
    UIHandler(UIHandler&&) = delete;
    UIHandler(const UIHandler&) = delete;

    /**
     * Loads default font into textures
     * @param fontPath path to the font file (usually ends with .ttf)
     * @param fontSize size of the font
     */
    static void Initialize(const std::string& fontPath, int fontSize);

    /**
     * Renders given string on the screen
     * @param shader shader to apply
     * @param text text to render
     * @param x x position on the screen
     * @param y y position on the screen
     * @param scale scale of the text comparing to the default font size
     * @param color color of the text
     */
    static void RenderText(Shader shader, const std::string &text, float x, float y, float scale, const glm::vec3 &color);
};


#endif //GRAPHICS_UIHANDLER_H
