#if !defined(TEXT_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define TEXT_H
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../shader/shader.h"
class text
{
public :
    text( std::string msg, glm::vec2 position, int size, bool reverse );
    void draw(shader shader);
    void setText( std::string newText );
private:
    std::string msg;
    glm::vec2 position;
    int size;
    bool reverse;
    float width;
    float height;
    GLuint texture;
    GLuint textureFromText();
};
#endif
