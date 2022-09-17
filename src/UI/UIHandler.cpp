//
// Created by Anton on 16.08.2022.
//

#include "UIHandler.h"
#include "../Core/Window.h"
#include "../Core/InGameException.h"
#include "../Logging/easylogging++.h"
#include "../vendors/include/glm/gtc/matrix_transform.hpp"

std::map<char, Character> UIHandler::Characters;
unsigned int UIHandler::VAO, UIHandler::VBO, UIHandler::loadedFontSize;


void UIHandler::Initialize(const std::string& fontPath, int fontSize)
{
    FT_Library ft = nullptr;

    // All functions return a value different from 0 whenever an error occurred
    if (FT_Init_FreeType(&ft) != 0)
    {
        LOG(ERROR) << "ERROR::FREETYPE: Could not Initialize FreeType Library";
        throw InGameException("FreeType library initialization error");
    }

    // load font as face
    FT_Face face = nullptr;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face) != 0)
    {
        LOG(ERROR) << "ERROR::FREETYPE: Failed to load font";
        throw InGameException("Failed to load font");
    }

    // set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, fontSize);

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
        {
            LOG(ERROR) << "ERROR::FREETYTPE::Failed to load Glyph for " << (char) c << " character";
            continue;
        }
        // generate texture
        unsigned int texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                static_cast<int>(face->glyph->bitmap.width),
                static_cast<int>(face->glyph->bitmap.rows),
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character =
        {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    // set loaded loaded font var
    loadedFontSize = fontSize;

    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    LOG(INFO) << "Type loaded, FreeType library terminated";
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void UIHandler::RenderText(Shader * shader, const std::string &text, float x, float y, int fontSize, const glm::vec3 &color)
{
    glDisable(GL_DEPTH_TEST);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(Window::GetWidth()), 0.0f, static_cast<float>(Window::GetHeight()));

    // activate corresponding render state
    shader->Use();
    shader->setMat4("projection", projection);
    shader->setVec3("textColor", color);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    float scale = static_cast<float>(fontSize) / static_cast<float>(loadedFontSize);

    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + static_cast<float>(ch.Bearing.x) * scale;
        float ypos = y - static_cast<float>((ch.Size.y - ch.Bearing.y)) * scale;

        float w = static_cast<float>(ch.Size.x) * scale;
        float h = static_cast<float>(ch.Size.y) * scale;

        // update VBO for each character
        float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (float) (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glEnable(GL_DEPTH_TEST);
}

void UIHandler::RenderTexture(Shader * shader, float x, float y, float w, float h, unsigned int texture)
{
    glDisable(GL_DEPTH_TEST);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(Window::GetWidth()), 0.0f, static_cast<float>(Window::GetHeight()));

    // activate corresponding render state
    shader->Use();
    shader->setMat4("projection", projection);
//    shader.setVec3("textColor", color);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    float xpos = x;
    float ypos = y;

    // update VBO for each character
    float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
    };

    // render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, texture);

    // update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glEnable(GL_DEPTH_TEST);
}
