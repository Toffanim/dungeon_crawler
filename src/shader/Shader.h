#if !defined(SHADER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice: $
   ======================================================================== */

#define SHADER_H

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string>
#include <map>
#include <GL/glew.h>
#include "../utils/Utils.h"

class Shader
{
public:
    Shader(std::string filename);
    //void init(std::map<int, std::string>& attr, std::map<int, std::string>& fragData);
    void init();
    GLuint getProgram() { return(program); }
    void use();
private :
    std::string filename;
    GLuint program;
    //void bindAttrib( std::map<int, std::string>& attrib );
    //void bindFragData( std::map<int, std::string>& fragData );
    void linkShaderProgram();
    GLuint loadShaderProgram( const std::string& vertex, const std::string& fragment);
};

#endif
