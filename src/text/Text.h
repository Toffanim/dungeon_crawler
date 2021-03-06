#if !defined(TEXT_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice:  $
   ======================================================================== */

#define TEXT_H
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../shader/Shader.h"

class Text
{
public :
    Text( std::string msg, glm::vec2 position, int size, bool reverse );
    void draw(Shader shader);
    void setText( std::string newText );
    std::string getText(){return(msg);}
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
