/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "game.h"

game::game()
{

}

void game::init()
{
    // SDL Init
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Erreur lors de l'initialisation de la SDL : " << SDL_GetError() << std::endl;
        SDL_Quit(); 
        return(-1);
    }

    // Set openGL versions    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    
    // Set Double Buffer rendering    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create SDL Wikndow
    window = SDL_CreateWindow("Test SDL 2.0", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN| SDL_WINDOW_OPENGL);
    if(window == 0)
    {
        std::cout << "Erreur lors de la creation de la fenetre : " << SDL_GetError() << std::endl;
        SDL_Quit();
        return(-1);
    }

    // Create OpenGL context
    openGLcontext = SDL_GL_CreateContext(window);
    if(openGLcontext == 0)
    {
        std::cout << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return(-1);
    }

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return(-1);
    }

    startupGLDiagnostics();

    // Vertices et coordonnées
    float vertices[] = {-0.5, -0.5,   0.0, 0.5,   0.5, -0.5};
    return(0);
}

int game::mainLoop()
{
    // Main loop
    while(!endgame)
    {
        SDL_WaitEvent(&events);

        if(events.window.event == SDL_WINDOWEVENT_CLOSE)
            endgame = true;

        // Actualisation de la fenêtre
        SDL_GL_SwapWindow(window);
    }

    // Deaalowing and cleaning app
    SDL_DestroyWindow(fenetre);
    SDL_Quit();
    return(0);
}