/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "text.h"
using namespace std;

text::text( std::string msg, glm::vec2 position)
        :msg(msg), position(position)
{
    texture = textureFromText();
}

void text::setText( std::string newText )
{
    if ( newText != msg )
    {
        msg = newText;
        cout << "set newText :" << msg << endl;
        texture = textureFromText();
    }
}

void text::draw(shader shader)
{
    int Width = 100;
    int Height = 50;
    // Set up vertex data (and buffer(s)) and attribute pointers
    /*  GLfloat vertices[] = {
    // Positions          // Colors           // Texture Coords
    0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top Right
    0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom Right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top Left 
    };
    */
    GLfloat vertices[] = {
        // Positions          // Colors           // Texture Coords
        position.x + Width,  position.y, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top Right
        position.x + Width, position.y + Height, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom Right
        position.x, position.y + Height, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
        position.x, position.y, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top Left 
    };


    GLuint indices[] = {  // Note that we start from 0!
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // TexCoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shader.getProgram(), "texture_diffuse1"), 0);  
        
    // Draw container
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

GLuint text::textureFromText()
{
    GLuint textureID;
    if( TTF_Init() == -1)
        cout << "probleme init ttf" << endl;
    TTF_Font* f = TTF_OpenFont( "../Assets/Fonts/angelina.TTF", 100);
    if (!f)
        cout << "probleme font" << endl;
    SDL_Surface* image = TTF_RenderText_Blended( f, msg.c_str(), {255,255,255,255});
    //image = flipSurface(image);
    TTF_CloseFont(f);
    TTF_Quit();
    //Generate texture ID and load texture data 
    glGenTextures(1, &textureID);
    if(!image)
    {
        cout << "Font load problem" << endl;
        return(0);
    }
    // Format de l'image
    GLenum formatInterne(0);
    GLenum format(0);
    cout << (int)image->format->BytesPerPixel << endl;
    // Détermination du format et du format interne pour les images à 3 composantes
    if(image->format->BytesPerPixel == 3)
    {
        // Format interne
        formatInterne = GL_RGB;
        // Format
        if(image->format->Rmask == 0xff)
            format = GL_RGB;
        else
            format = GL_BGR;
    }
    // Détermination du format et du format interne pour les images à 4 composantes
    else if(image->format->BytesPerPixel == 4)
    {
        // Format interne
        formatInterne = GL_RGBA;
        // Format
        if(image->format->Rmask == 0xff)
            format = GL_RGBA;
        else
            format = GL_BGRA;
    }
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);    

    // Parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SDL_FreeSurface(image);
    return textureID;
}
