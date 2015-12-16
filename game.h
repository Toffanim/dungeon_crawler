#if !defined(GAME_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define GAME_H

#include <stdlib.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "player.h"
#include "controller.h"
#include "Model.h"
#include "camera.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class game
{
public :
    game(int width, int height);
    int init();
    void close();
    int mainLoop();
private :
    SDL_Window* window;
    SDL_GLContext openGLcontext;
    SDL_Event events;
    bool endgame;
    int screenWidth;
    int screenHeight;

    void startupGLDiagnostics();
};

#endif
