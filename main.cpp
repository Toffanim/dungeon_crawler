/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "main.h"
using namespace std;

#if 0
GLuint loadShaderProgram(const std::string &vertexShader, const std::string &fragmentShader)
{
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char *vs = textFileRead(vertexShader.c_str());
    const char *fs = textFileRead(fragmentShader.c_str());

    glShaderSource(vShader, 1, &vs, NULL);
    glShaderSource(fShader, 1, &fs, NULL);
    // text data is not needed beyond this point
    delete [] vs;
    delete [] fs;

    glCompileShader(vShader);
    int compileOk = 0;
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &compileOk);
    if (!compileOk) 
    {
        printf("Compile shader error");
        return 0;
    }

    glCompileShader(fShader);
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &compileOk);
    if (!compileOk) 
    {
        printf("Compile shader error");
        return 0;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, fShader);
    glDeleteShader( fShader );
    glAttachShader(shaderProgram, vShader);
    glDeleteShader( vShader );

    return shaderProgram; 
}
#endif

void linkShaderProgram(GLuint shaderProgram)
{
    glLinkProgram(shaderProgram);
    GLint linkOk = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkOk);
    if (!linkOk)
    {
        printf("Link shader error");
        return;
    }
}

int main(int argc, char** argv)
{

    game* mygame = new game();
    mygame->init();
    mygame->mainLoop();

    
#if 0
    GLuint shaderProgram = loadShaderProgram("Shaders/shading.vert", "Shaders/shading.frag");
    glBindAttribLocation(shaderProgram, 0, "position"); 
    glBindAttribLocation(shaderProgram, 1, "normalIn");
    glBindAttribLocation(shaderProgram, 2, "texCoordIn");       
    glBindFragDataLocation(shaderProgram, 0, "fragmentColor");
    linkShaderProgram(shaderProgram);


    GLuint simpleShaderProgram = loadShaderProgram("Shaders/shadowMap.vert", "Shaders/shadowMap.frag");
    glBindAttribLocation(simpleShaderProgram, 0, "position");
    glBindFragDataLocation(simpleShaderProgram, 0, "fragmentColor");
    linkShaderProgram(simpleShaderProgram);
#endif


    return 0;

}
