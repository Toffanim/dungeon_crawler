/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "main.h"
using namespace std;

const char *textFileRead( const char *fn )
{
    /* Note: the `fatalError' thing is a bit of a hack, The proper course of
     * action would be to have people check the return values of textFileRead,
     * but lets avoid cluttering the lab-code even further.
     */

    FILE *fp;
    char *content = NULL;
    int count = 0;

    if( fn != NULL ) 
    {
        fp = fopen( fn, "rt" );
        if( fp != NULL ) 
        {
            fseek( fp, 0, SEEK_END );
            count = ftell( fp );
            fseek( fp, 0, SEEK_SET );

            if( count > 0 ) 
            {
                content = new char[count+1];
                count = fread( content, sizeof(char), count, fp );
                content[count] = '\0';
            }
            else
            {
                printf("Error : file empty");
            }

            fclose( fp );
        }
        else
        {
            printf("Error : Cant read file");
        }
    }
    else
    {
        printf("Error : NULL arg");
    }

    return content;
}


void startupGLDiagnostics()
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

GLuint loadShaderProgram(const std::string &vertexShader, const std::string &fragmentShader)
{
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char *vs = textFileRead(vertexShader.c_str());
    const char *fs = textFileRead(fragmentShader.c_str());

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


void linkShaderProgram(GLuint shaderProgram)
{
    glLinkProgram(shaderProgram);
    GLint linkOk = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkOk);
    if (!linkOk)
    {
        printf("Link shader error");
        return;
    }
}

int main(int argc, char** argv)
{
    // Create SDL Windows Handler
    SDL_Window* fenetre(0);
    SDL_GLContext contexteOpenGL(0);
    SDL_Event evenements;
    bool terminer(false);

    // SDL Init
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Erreur lors de l'initialisation de la SDL : " << SDL_GetError() << std::endl;
        SDL_Quit(); 
        return -1;
    }

    // Set openGL versions    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    
    // Set Double Buffer rendering    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create SDL Wikndow
    fenetre = SDL_CreateWindow("Test SDL 2.0", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN| SDL_WINDOW_OPENGL);
    if(fenetre == 0)
    {
        std::cout << "Erreur lors de la creation de la fenetre : " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Create OpenGL context
    contexteOpenGL = SDL_GL_CreateContext(fenetre);
    if(contexteOpenGL == 0)
    {
        std::cout << SDL_GetError() << std::endl;
        SDL_DestroyWindow(fenetre);
        SDL_Quit();
        return -1;
    }

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    startupGLDiagnostics();

    // Vertices et coordonnées
    float vertices[] = {-0.5, -0.5,   0.0, 0.5,   0.5, -0.5};

#if 0
    GLuint shaderProgram = loadShaderProgram("Shaders/shading.vert", "Shaders/shading.frag");
    glBindAttribLocation(shaderProgram, 0, "position"); 
    glBindAttribLocation(shaderProgram, 1, "normalIn");
    glBindAttribLocation(shaderProgram, 2, "texCoordIn");       
    glBindFragDataLocation(shaderProgram, 0, "fragmentColor");
    linkShaderProgram(shaderProgram);


    GLuint simpleShaderProgram = loadShaderProgram("Shaders/shadowMap.vert", "Shaders/shadowMap.frag");
    glBindAttribLocation(simpleShaderProgram, 0, "position");
    glBindFragDataLocation(simpleShaderProgram, 0, "fragmentColor");
    linkShaderProgram(simpleShaderProgram);
#endif
    // Main loop
    while(!terminer)
    {
        SDL_WaitEvent(&evenements);

        if(evenements.window.event == SDL_WINDOWEVENT_CLOSE)
            terminer = true;

        // Nettoyage de l'écran
        glClear(GL_COLOR_BUFFER_BIT);

        // On remplie puis on active le tableau Vertex Attrib 0
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        glEnableVertexAttribArray(0);

        // On affiche le triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // On désactive le tableau Vertex Attrib puisque l'on n'en a plus besoin
        glDisableVertexAttribArray(0);

        // Actualisation de la fenêtre
        SDL_GL_SwapWindow(fenetre);
    }

    // Deaalowing and cleaning app
    SDL_DestroyWindow(fenetre);
    SDL_Quit();
    return 0;

}
