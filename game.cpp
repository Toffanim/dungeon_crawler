/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "game.h"



using namespace std;

game::game(int width, int height) : screenWidth(width), screenHeight(height)
{
}


void game::startupGLDiagnostics()
{
    // print diagnostic information
    printf( "GL  VENDOR: %s\n", glGetString( GL_VENDOR ) );
    printf( "   VERSION: %s\n", glGetString( GL_VERSION ) );
    printf( "  RENDERER: %s\n", glGetString( GL_RENDERER ) );

    // test if we've got GL 3.0
    if( !GLEW_VERSION_3_0 )
    {
        printf( "OpenGL 3.0 not supported.\n" 
            "Please update your drivers and/or buy a better graphics card."
        );
    }
}

int game::init()
{
    // SDL Init
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Erreur lors de l'initialisation de la SDL : " << SDL_GetError() << std::endl;
        SDL_Quit(); 
        return(-1);
    }
    SDL_SetRelativeMouseMode(SDL_TRUE);
    // Set openGL versions    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    
    // Set Double Buffer rendering    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create SDL Wikndow
    window = SDL_CreateWindow("Dungeon Crawler", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN| SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_GRABBED);
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

void game::close()
{
    endgame = true;
}

int game::mainLoop()
{
    player* p = new player();
    p->getController()->setMiscCallback(SDL_WINDOWEVENT_CLOSE, std::bind(&game::close, this));
    shader* s = new shader( "Shaders/simple" );
    s->init();
    Model* m = new Model( "Alien_Necromorph/Alien_Necromorph.obj" );
    float last = 0.0f;
    float deltaTime = 0.0f;
    float current = 0.0f;
// Main loop
    while(!endgame)
    {
        current = SDL_GetTicks();
        deltaTime = (current - last)/1000;
        last = current;
        p->getController()->processEvents();
        p->move(deltaTime);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        s->use();
        glm::mat4 projectionMatrix = glm::perspective(p->getCamera()->getZoom(), (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
        glm::mat4 viewMatrix = p->getCamera()->getViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(s->getProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
               glUniformMatrix4fv(glGetUniformLocation(s->getProgram(), "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));

        // Draw the loaded model
        glm::mat4 model;
        model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f)); // Translate it down a bit so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f)); // It's a bit too big for our scene, so scale it down
        glUniformMatrix4fv(glGetUniformLocation(s->getProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));

        m->Draw(*s);
        // Actualisation de la fenêtre
        SDL_GL_SwapWindow(window);
    }

    // Deaalowing and cleaning app
    SDL_DestroyWindow(window);
    SDL_Quit();
    return(0);
}
