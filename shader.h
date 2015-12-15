#if !defined(SHADER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define SHADER_H

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string>
#include <map>
#include <GL/glew.h>
#include "utils.h"

class shader
{
public:
    shader(std::string filename);
    void init(std::map<int, std::string>& attr, std::map<int, std::string>& fragData);
    GLuint getProgram() { return(program); }
private :
    std::string filename;
    GLuint program;
    void bindAttrib( std::map<int, std::string>& attrib );
    void bindFragData( std::map<int, std::string>& fragData );
    void linkShaderProgram();
    GLuint loadShaderProgram( const std::string& vertex, const std::string& fragment);
};

#endif
