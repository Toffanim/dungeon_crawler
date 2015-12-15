#if !defined(UTILS_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace utils
{
    const char* textFileRead( const char *fn );
    GLuint TextureFromFile( const char* path, std::string directory);
};
#endif
