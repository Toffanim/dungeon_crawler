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

class game
{
public :
    game();
    void init();
    int mainLoop();
private :
    SDL_Window* window(0);
    SDL_GLContext openGLcontext(0);
    SDL_Event events;
    bool endgame;
}
