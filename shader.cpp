/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "shader.h"
using namespace std;

shader::shader(string filename):filename(filename)
{

}

void shader::init(map<int, string>* attr, map<int, string>* fragData)
{
    string vertex = filename + ".vert";
    string fragment = filename + ".frag";
    program = loadShaderProgram(vertex, fragment);
    bindAttrib(attr );
    bindFragData(fragData);
    linkShaderProgram();
}


GLuint shader::loadShaderProgram(const std::string &vertexShader, const std::string &fragmentShader)
{
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char *vs = utils::textFileRead(vertexShader.c_str());
    const char *fs = utils::textFileRead(fragmentShader.c_str());

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


void shader::linkShaderProgram()
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


void shader::bindAttrib( std::map<int, std::string>* attrib )
{
    map<int, string>::iterator it;
    for (it = attrib->begin();
         it != attrib->end();
         ++it)
    {
        glBindAttribLocation( program, it->first, it->second.c_str());
    }
}

void shader::bindFragData(std::map<int, std::string>* fragData )
{
    map<int, string>::iterator it;
    for (it = fragData->begin();
         it != fragData->end();
         ++it)
    {
        glBindFragDataLocation( program, it->first, it->second.c_str());
    }
}
