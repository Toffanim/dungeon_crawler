/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "shader.h"

shader::shader(string filename):filename(filename)
{

}

void shader::init()
{
    string vertex = filename + ".vert";
    string fragment = filename + ".frag";
    program = loadShaderProgram(vertex, fragment);
    bindAttrib( /*map*/ );
    bindFragData( /*map*/ );
    linkShaderProgram();
}


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


void linkShaderProgram()
{
    glLinkProgram(program);
    GLint linkOk = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linkOk);
    if (!linkOk)
    {
        printf("Link shader error");
        return;
    }
}


void bindAttrib( map attrib)
{
    //TODO(marc): make map works to have map[nb]=strings
    for (int i = 0;
         i < attrib.size();
         ++i)
    {
        glBindAttribLocation( program, map[i], map[i]);
    }
}

void bindFragData( map fragData )
{
//TODO(marc) : make map works
    for (int i = 0;
         i < fragData.size();
         ++i)
    {
        glBindFragDataLocation( program, map[i], map[i]);
    }
}
