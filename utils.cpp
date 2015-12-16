/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "utils.h"
#include <iostream>
namespace utils {
    using namespace std;

    GLuint TextureFromFile( const char* path, string directory)
    {
        //Generate texture ID and load texture data 
        string filename = string(path);
        filename = directory + '/' + filename;
        GLuint textureID;
        glGenTextures(1, &textureID);
        SDL_Surface *image = IMG_Load(filename.c_str());
        if(!image)
        {
            cout << "IMG_Load : " << IMG_GetError() << endl;
            return(0);
        }
        // Format de l'image
        GLenum formatInterne(0);
        GLenum format(0);
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
        glTexImage2D(GL_TEXTURE_2D, 0, formatInterne, image->w, image->h, 0, format, GL_UNSIGNED_BYTE, image);
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
};
