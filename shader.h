#if !defined(SHADER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define SHADER_H
#endif

class shader
{
public:
    shader();
    void init();
private :
    std::string filename;
    GLuint program;
    void bindAttrib( map attrib );
    void bindFragData( map fragData );
    void linkShaderProgram();
    void loadShaderProgram( const string& vertex, const string& fragment);
};
