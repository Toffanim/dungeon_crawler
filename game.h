#if !defined(GAME_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define GAME_H
#endif

#include <stdlib.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "shader.h"

class game
{
public :
    game();
    int init();
    int mainLoop();
private :
    SDL_Window* window;
    SDL_GLContext openGLcontext;
    SDL_Event events;
    bool endgame;

    void startupGLDiagnostics();
};
