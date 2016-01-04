
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "game.h"
#define PI 3.14159265359f

using namespace std;

game::game(int width, int height) : screenWidth(width),
                                    screenHeight(height),
                                    endgame(false)
{
    init();
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
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

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
    //glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);    // enable Z-buffering
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    return(0);
}

void game::close()
{
    endgame = true;
}

struct RGB
{
    unsigned char r, g, b;
};

struct ImageRGB
{
    int w, h;
    vector<RGB> data;
};

void eat_comment(ifstream &f)
{
    char linebuf[1024];
    char ppp;
    while (ppp = f.peek(), ppp == '\n' || ppp == '\r')
        f.get();
    if (ppp == '#')
        f.getline(linebuf, 1023);
}

void load_ppm(ImageRGB &img, const string &name)
{
    ifstream f(name.c_str(), ios::binary);
    if (f.fail())
    {
        cout << "Could not open file: " << name << endl;
        return;
    }

    // get type of file
    eat_comment(f);
    int mode = 0;
    string s;
    f >> s;
    if (s == "P3")
        mode = 3;
    else if (s == "P6")
        mode = 6;
    
    // get w
    eat_comment(f);
    f >> img.w;

    // get h
    eat_comment(f);
    f >> img.h;
    
    // get bits
    eat_comment(f);
    int bits = 0;
    f >> bits;
    
    // error checking
    if (mode != 3 && mode != 6)
    {
        cout << "Unsupported magic number" << endl;
        f.close();
        return;
    }
    if (img.w < 1)
    {
        cout << "Unsupported width: " << img.w << endl;
        f.close();
        return;
    }
    if (img.h < 1)
    {
        cout << "Unsupported height: " << img.h << endl;
        f.close();
        return;
    }
    if (bits < 1 || bits > 255)
    {
        cout << "Unsupported number of bits: " << bits << endl;
        f.close();
        return;
    }

    // load image data
    img.data.resize(img.w * img.h);

    if (mode == 6)
    {
        f.get();
        f.read((char*)&img.data[0], img.data.size() * 3);
    }
    else if (mode == 3)
    {
        for (unsigned int i = 0; i < img.data.size(); i++)
        {
            int v;
            f >> v;
            img.data[i].r = v;
            f >> v;
            img.data[i].g = v;
            f >> v;
            img.data[i].b = v;
        }
    }

    // close file
    f.close();
}


struct SceneNode
{
    Model* model;
    glm::mat4 modelMatrix;    
};

struct room
{
    bool murSud;
    bool murEst;
    bool murOuest;
    bool murNord;
    int offsetX;
    int offsetY;
};


void makeWaterRoom(vector<actor*>& scene, room& r, room& r2)
{
    // if (wallNorth) ...
    // coord : 1.0 = 1m
    //scal = tileSize

    modelManager& mm = modelManager::getInstance();

    int ox = r.offsetX;
    int oy = r.offsetY;

    //sol
    
    glm::mat4 modelMatrix = glm::mat4();
    modelMatrix = glm::rotate(modelMatrix, (PI/2.0f), glm::vec3(1.0f,0.0f,0.0f));
    glm::vec3 position = glm::vec3(ox, oy, 1.0f);
    modelMatrix = glm::translate(modelMatrix, position);    

    scene.push_back(
        new wall( mm.getModels()["wall"], modelMatrix ) );


    //sol
    
    modelMatrix = glm::mat4();
    modelMatrix = glm::rotate(modelMatrix, (PI/2.0f), glm::vec3(1.0f,0.0f,0.0f));
    position = glm::vec3(ox, oy, 0.0f);
    modelMatrix = glm::translate(modelMatrix, position);    

    scene.push_back(
        new water(mm.getModels()["water"], modelMatrix ) );
    
    //plafond
    modelMatrix = glm::mat4();
    modelMatrix = glm::rotate(modelMatrix, -(PI/2.0f), glm::vec3(1.0f,0.0f,0.0f));
    position = glm::vec3(ox, -(oy+1)*1.0f, 1.0f);
    modelMatrix = glm::translate( modelMatrix, position );
    scene.push_back(
        new wall( mm.getModels()["wall"], modelMatrix) );


    if(r2.murNord)
    {
    //mur sud
    modelMatrix = glm::mat4();
    modelMatrix = glm::rotate(modelMatrix, PI, glm::vec3(0.0f,1.0f,0.0f));
    position = glm::vec3(-(ox+1)*1.0f, -1.0f, (-oy));
    modelMatrix = glm::translate(modelMatrix, position);
    scene.push_back(
    new wall( mm.getModels()["wall"], modelMatrix) );
}
    //mur nord
    if(r2.murSud)
    {
        
    modelMatrix = glm::mat4();
    position = glm::vec3(ox, -1.0f, (oy+1)*1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    scene.push_back(
    new wall( mm.getModels()["wall"], modelMatrix) );
    }
    if(r2.murOuest)
    {
        //mur 
    modelMatrix = glm::mat4();
        modelMatrix = glm::rotate( modelMatrix, -(PI/2.0f), glm::vec3( 0.0f, 1.0f, 0.0f));
        position = glm::vec3(oy, -1.0f, -ox);
        modelMatrix = glm::translate( modelMatrix, position);
        scene.push_back(
    new wall( mm.getModels()["wall"], modelMatrix) );
    }
    if(r2.murEst)
    {
        //mur ouest
        modelMatrix = glm::mat4();
        modelMatrix = glm::rotate( modelMatrix, (PI/2.0f), glm::vec3( 0.0f, 1.0f, 0.0f));
        position = glm::vec3(-(oy+1)*1.0f, -1.0f, (ox+1)*1.0f);
        modelMatrix = glm::translate( modelMatrix, position);
        scene.push_back(
            new wall( mm.getModels()["wall"], modelMatrix) );
    }

    
    if(r.murNord)
    {
    //mur sud
    modelMatrix = glm::mat4();
    modelMatrix = glm::rotate(modelMatrix, PI, glm::vec3(0.0f,1.0f,0.0f));
    position = glm::vec3(-(ox+1)*1.0f, 0.0f, (-oy));
    modelMatrix = glm::translate(modelMatrix, position);
    scene.push_back(
    new wall( mm.getModels()["wall"], modelMatrix) );
}
    //mur nord
    if(r.murSud)
    {
        
    modelMatrix = glm::mat4();
    position = glm::vec3(ox, 0.0f, (oy+1)*1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    scene.push_back(
    new wall( mm.getModels()["wall"], modelMatrix) );
    }
    if(r.murOuest)
    {
        //mur 
    modelMatrix = glm::mat4();
        modelMatrix = glm::rotate( modelMatrix, -(PI/2.0f), glm::vec3( 0.0f, 1.0f, 0.0f));
        position = glm::vec3(oy, 0.0f, -ox);
        modelMatrix = glm::translate( modelMatrix, position);
        scene.push_back(
    new wall( mm.getModels()["wall"], modelMatrix) );
    }
    if(r.murEst)
    {
        //mur ouest
        modelMatrix = glm::mat4();
        modelMatrix = glm::rotate( modelMatrix, (PI/2.0f), glm::vec3( 0.0f, 1.0f, 0.0f));
        position = glm::vec3(-(oy+1)*1.0f, 0.0f, (ox+1)*1.0f);
        modelMatrix = glm::translate( modelMatrix, position);
        scene.push_back(
            new wall( mm.getModels()["wall"], modelMatrix) );
    }
}



void makeRoom(vector<actor*>& scene, room& r)
{
    // if (wallNorth) ...
    // coord : 1.0 = 1m
    //scal = tileSize

    modelManager& mm = modelManager::getInstance();

    int ox = r.offsetX;
    int oy = r.offsetY;

    //sol
    glm::mat4 modelMatrix = glm::mat4();
    modelMatrix = glm::rotate(modelMatrix, (PI/2.0f), glm::vec3(1.0f,0.0f,0.0f));
    glm::vec3 position = glm::vec3(ox, oy, 0.0f);
    modelMatrix = glm::translate(modelMatrix, position);    

    scene.push_back(
        new wall( mm.getModels()["wall"], modelMatrix ) );
    
    //plafond
    modelMatrix = glm::mat4();
    modelMatrix = glm::rotate(modelMatrix, -(PI/2.0f), glm::vec3(1.0f,0.0f,0.0f));
    position = glm::vec3(ox, -(oy+1)*1.0f, 1.0f);
    modelMatrix = glm::translate( modelMatrix, position );
    scene.push_back(
        new wall( mm.getModels()["wall"], modelMatrix) );

    if(r.murNord)
    {
    //mur sud
    modelMatrix = glm::mat4();
    modelMatrix = glm::rotate(modelMatrix, PI, glm::vec3(0.0f,1.0f,0.0f));
    position = glm::vec3(-(ox+1)*1.0f, 0.0f, (-oy));
    modelMatrix = glm::translate(modelMatrix, position);
    scene.push_back(
    new wall( mm.getModels()["wall"], modelMatrix) );
}
    //mur nord
    if(r.murSud)
    {
        
    modelMatrix = glm::mat4();
    position = glm::vec3(ox, 0.0f, (oy+1)*1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    scene.push_back(
    new wall( mm.getModels()["wall"], modelMatrix) );
    }
    if(r.murOuest)
    {
        //mur 
    modelMatrix = glm::mat4();
        modelMatrix = glm::rotate( modelMatrix, -(PI/2.0f), glm::vec3( 0.0f, 1.0f, 0.0f));
        position = glm::vec3(oy, 0.0f, -ox);
        modelMatrix = glm::translate( modelMatrix, position);
        scene.push_back(
    new wall( mm.getModels()["wall"], modelMatrix) );
    }
    if(r.murEst)
    {
        //mur ouest
        modelMatrix = glm::mat4();
        modelMatrix = glm::rotate( modelMatrix, (PI/2.0f), glm::vec3( 0.0f, 1.0f, 0.0f));
        position = glm::vec3(-(oy+1)*1.0f, 0.0f, (ox+1)*1.0f);
        modelMatrix = glm::translate( modelMatrix, position);
        scene.push_back(
            new wall( mm.getModels()["wall"], modelMatrix) );
    }
}


bool isColorEquals( glm::vec3 c1, glm::vec3 c2)
{
    if ( c1.x == c2.x && c1.y == c2.y && c1.z == c2.z)
        return (true);
    return (false);
}

void loadMaze( const string &filename, vector<actor*>& scene, player* p)
{
    ImageRGB img;
    load_ppm(img, filename);
    for( int i = 0;
         i < img.h;
         ++i)
    {
        for ( int j = 0;
              j < img.w;
              ++j )
        {
            int idx = (img.w *i)+j;
            if ( isColorEquals( glm::vec3 ( img.data[idx].r, img.data[idx].g, img.data[idx].b),
                                glm::vec3(255,255,255)))
            {
                room r;
                r.offsetX = j;
                r.offsetY = i;
                r.murOuest = false;
                r.murEst = false;
                r.murNord = false;
                r.murSud = false;
//check north connexion
                int nidx = idx-(img.w);
                if ( nidx < 0 || isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                                glm::vec3(0,0,0)))
                {
                    r.murNord = true;
                    //cout << "mur nord" << endl;
                }
                nidx = idx+(img.w);
                if ( nidx >= img.data.size() || isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                                               glm::vec3(0,0,0))) 
                {
                    r.murSud = true;
                    //cout << "mur sud" << endl;
                }
                nidx = idx-1;
                if ( nidx < 0 || isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                                glm::vec3(0,0,0))) 
                {
                    r.murOuest = true;
                    // cout << "mur ouest" << endl;
                }
                nidx = idx+1;
                if ( nidx >= img.data.size() || isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                                               glm::vec3(0,0,0))) 
                {
                    r.murEst = true;
                    //    cout << "mur est" << endl;
                }
                makeRoom(scene,r);
            }

            if( isColorEquals( glm::vec3 ( img.data[idx].r, img.data[idx].g, img.data[idx].b),
                               glm::vec3(255,0,0)))
            {
                p->setPosition( glm::vec3( j+(0.5), 0.5, i+(0.5) ) );
                //cout << "changePosition" << endl;
                if ( idx % img.w == 0)
                {//bord gauche
                    // cout << "bord gauche" << endl;
                    p->getCamera()->addYaw(-90.0);
                }
                else if ( idx % img.w == img.w-1)
                {   //bord droit
                    //                  cout << "bord droit" << endl;
                    p->getCamera()->addYaw(90.0);
                }
                else if ( idx < img.w )
                {
//                    cout << "bord bas" << endl;                    
                }
                else
                {
                    //cout << "bord haut" << endl;
                    p->getCamera()->addYaw(-180.0);
                }
                //TODO(marc) : FIX ME !!! + add makeRoom to entry and exit tiles
                //recul le player de moitier
                p->setPosition( p->getPosition() );

                room r;
                r.offsetX = j;
                r.offsetY = i;
                r.murOuest = false;
                r.murEst = false;
                r.murNord = false;
                r.murSud = false;
//check north connexion
                int nidx = idx-(img.w);
                if ( nidx < 0 || isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                                glm::vec3(0,0,0)))
                {
                    r.murNord = true;
                    //cout << "mur nord" << endl;
                }
                nidx = idx+(img.w);
                if ( nidx >= img.data.size() || isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                                               glm::vec3(0,0,0))) 
                {
                    r.murSud = true;
                    //cout << "mur sud" << endl;
                }
                nidx = idx-1;
                if ( nidx < 0 || isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                                glm::vec3(0,0,0))) 
                {
                    r.murOuest = true;
                    // cout << "mur ouest" << endl;
                }
                nidx = idx+1;
                if ( nidx >= img.data.size() || isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                                               glm::vec3(0,0,0))) 
                {
                    r.murEst = true;
                    //    cout << "mur est" << endl;
                }
                makeRoom(scene,r);
            }



            if ( isColorEquals( glm::vec3( img.data[idx].r, img.data[idx].g, img.data[idx].b),
                                glm::vec3(0,0,255)))
            {
                room r;
                r.offsetX = j;
                r.offsetY = i;
                r.murOuest = false;
                r.murEst = false;
                r.murNord = false;
                r.murSud = false;
//check north connexion
                int nidx = idx-(img.w);
                if ( nidx < 0 || isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                                glm::vec3(0,0,0)))
                {
                    r.murNord = true;
                    //cout << "mur nord" << endl;
                }
                nidx = idx+(img.w);
                if ( nidx >= img.data.size() || isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                                               glm::vec3(0,0,0))) 
                {
                    r.murSud = true;
                    //cout << "mur sud" << endl;
                }
                nidx = idx-1;
                if ( nidx < 0 || isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                                glm::vec3(0,0,0))) 
                {
                    r.murOuest = true;
                    // cout << "mur ouest" << endl;
                }
                nidx = idx+1;
                if ( nidx >= img.data.size() || isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                                               glm::vec3(0,0,0))) 
                {
                    r.murEst = true;
                    //    cout << "mur est" << endl;
                }

                room r2;
                r2.offsetX = j;
                r2.offsetY = i;
                r2.murOuest = false;
                r2.murEst = false;
                r2.murNord = false;
                r2.murSud = false;
//check north connexion
                nidx = idx-(img.w);
                if ( nidx < 0 || !isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                                glm::vec3(0,0,255)))
                {
                    r2.murNord = true;
                    //cout << "mur nord" << endl;
                }
                nidx = idx+(img.w);
                if ( nidx >= img.data.size() || !isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                                               glm::vec3(0,0,255))) 
                {
                    r2.murSud = true;
                    //cout << "mur sud" << endl;
                }
                nidx = idx-1;
                if ( nidx < 0 || !isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                                glm::vec3(0,0,255))) 
                {
                    r2.murOuest = true;
                    // cout << "mur ouest" << endl;
                }
                nidx = idx+1;
                if ( nidx >= img.data.size() || !isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                                               glm::vec3(0,0,255))) 
                {
                    r2.murEst = true;
                    //    cout << "mur est" << endl;
                }

                //NOTE(marc) : r2 is for under water
                makeWaterRoom( scene, r, r2 );

                
            }


            if ( isColorEquals( glm::vec3 ( img.data[idx].r, img.data[idx].g, img.data[idx].b),
                                glm::vec3(255,126,0)))
            {
                //door
                //NOTE(marc): we should check if door is valid or on image border
                //to be more accurate.
                room r;
                r.offsetX = j;
                r.offsetY = i;
                r.murOuest = false;
                r.murEst = false;
                r.murNord = false;
                r.murSud = false;
//check north connexion
                int nidx = idx-(img.w);
                if ( nidx < 0 || isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                                glm::vec3(0,0,0)))
                {
                    r.murNord = true;
                    //cout << "mur nord" << endl;
                }
                nidx = idx+(img.w);
                if ( nidx >= img.data.size() || isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                                               glm::vec3(0,0,0))) 
                {
                    r.murSud = true;
                    //cout << "mur sud" << endl;
                }
                nidx = idx-1;
                if ( nidx < 0 || isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                                glm::vec3(0,0,0))) 
                {
                    r.murOuest = true;
                    // cout << "mur ouest" << endl;
                }
                nidx = idx+1;
                if ( nidx >= img.data.size() || isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                                               glm::vec3(0,0,0))) 
                {
                    r.murEst = true;
                    //    cout << "mur est" << endl;
                }
                makeRoom(scene,r);
                nidx = idx-1;
                int nidx2 = idx+1;
                if(  isColorEquals( glm::vec3 ( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b),
                                    glm::vec3(0,0,0))
                     &&  isColorEquals( glm::vec3 ( img.data[nidx2].r, img.data[nidx2].g, img.data[nidx2].b),
                                        glm::vec3(0,0,0)))
                {
//door along x axis
                    cout << "door x axis" << endl;
                    modelManager& mm = modelManager::getInstance();
                    glm::vec3 position = glm::vec3(j, -0.1f, (i+0.5f)*1.0f);
                    glm::mat4 modelMatrix = glm::mat4();
                    modelMatrix = glm::translate(modelMatrix, position);
                    
                    scene.push_back(
                        new door( mm.getModels()["door"], modelMatrix )); 
                }
                else
                {
                   //door along y axis
                    cout << "door y axis" << endl;
                    modelManager& mm = modelManager::getInstance();
                    glm::mat4 modelMatrix = glm::mat4();
                    modelMatrix = glm::rotate( modelMatrix, (PI/2.0f), glm::vec3( 0.0f, 1.0f, 0.0f));         
                    glm::vec3 position = glm::vec3(-(i+1)*1.0f, -0.1f, (j+0.5)*1.0f);
                    modelMatrix = glm::translate( modelMatrix, position);
                    scene.push_back(
                        new door( mm.getModels()["door"], modelMatrix) );
                }
                
            }
        }
    }
}

void drawScene( vector<actor*>& scene, shader* mainShader, shader* waterShader)
{
    for ( vector<actor*>::iterator it = scene.begin();
    it != scene.end();
    ++it)
    {
        //TODO(marc) : make instancied rendering for walls ?
        if((*it)->getType() == "water")
        {
            if(waterShader)
            {
                waterShader->use();
                glUniformMatrix4fv(glGetUniformLocation(waterShader->getProgram(), "model"),
                                   1, GL_FALSE, glm::value_ptr((*it)->getModelMatrix()));
                (*it)->getModel()->Draw(*waterShader);
                mainShader->use();
            }
        }
        else
        {
            glUniformMatrix4fv(glGetUniformLocation(mainShader->getProgram(), "model"),
                               1, GL_FALSE, glm::value_ptr((*it)->getModelMatrix()));
            (*it)->getModel()->Draw(*mainShader);
        }
    }
}

bool isGLError()
{
    if( glGetError() != GL_NO_ERROR )
        return true;
    return false;
}

bool AABBtoAABB(const AABB& tBox1, const AABB& tBox2)
{

//Check if Box1's max is greater than Box2's min and Box1's min is less than Box2's max
    return(tBox1.max.x > tBox2.min.x &&
    tBox1.min.x < tBox2.max.x &&
    tBox1.max.y > tBox2.min.y &&
    tBox1.min.y < tBox2.max.y &&
    tBox1.max.z > tBox2.min.z &&
    tBox1.min.z < tBox2.max.z);

//If not, it will return false

}

void checkCollision(vector<actor*>& scene, player* p, float deltaTime)
{
    for( vector<actor*>::iterator it = scene.begin();
         it != scene.end();
         ++it)
    {
        if ( AABBtoAABB( (*it)->getAABB(), p->getAABB()))
        {
            (*it)->doCollision(p, deltaTime);
        }
    }
}

int game::mainLoop()
{
    vector<actor*> scene;
    player* p = new player();
    p->getController()->setMiscCallback(SDL_WINDOWEVENT_CLOSE, std::bind(&game::close, this));
    p->getController()->setKeyPressCallback(SDLK_ESCAPE, std::bind(&game::close, this));
#if 1
    modelManager& manager = modelManager::getInstance();
    manager.getModels().insert(
        std::pair<std::string, Model*>
        ("chest", new Model( "../Assets/Models/Chest/treasure_chest.obj")));
    manager.getModels().insert(
        std::pair<std::string, Model*>
        ("wall", new Model( "../Assets/Models/Wall/wall.obj" )));
    manager.getModels().insert(
        std::pair<std::string, Model*>
        ("door", new Model( "../Assets/Models/Door/door.obj" )));
    manager.getModels().insert(
        std::pair<std::string, Model*>
        ("monster", new Model( "../Assets/Models/Alien_Necromorph/Alien_Necromorph.obj" )));
    manager.getModels().insert(
        std::pair<std::string, Model*>
        ("water", new Model( "../Assets/Models/Water/water.obj" )));
    loadMaze( "../Assets/mazeTest.ppm", scene, p);

    glm::mat4 modelMatrix = glm::mat4();
    modelMatrix = glm::translate(modelMatrix, glm::vec3(p->getPosition().x, -0.1f, p->getPosition().z - 2.0f));
    modelMatrix = glm::scale( modelMatrix, glm::vec3(0.1,0.1,0.1));    
    
    chest* aChest = new chest( manager.getModels()["chest"], modelMatrix, chest::Type::GOLD, 100 );
    monster* m = new monster( manager.getModels()["monster"], modelMatrix,
                              glm::vec3(p->getPosition().x, -0.1f, p->getPosition().z - 2.0f), 2.0);
    scene.push_back(m);
    
    shader* lightningShader = new shader("../Shaders/simpleLight" );
    lightningShader->init();
    shader* textShader = new shader("../Shaders/simple" );
    textShader->init();
    shader* depthShader = new shader("../Shaders/simpleDepth");
    depthShader->init();
    shader * debugShader = new shader( "../Shaders/debug");
    debugShader->init();
    shader* waterShader = new shader( "../Shaders/water" );
    waterShader->init();
    text* test = new text( "DEFAULT", glm::vec2(0.0,0.0));
    //NOTE(marc) : rajouter un sens au texte pour pouvoir mettre la position
    //dans les coin a droite par exemple
    //Rajouter aussi une taille au texte (changement au moment du loadfont)
    //peut etre pouvoir donner une font aussi, peut etre pas util pourn le moment
    text* framerate_t = new text( "DEFAULT", glm::vec2(700.0, 0.0));
    
    float last = 0.0f;
    float deltaTime = 0.0f;
    float current = 0.0f;
#endif

    GLuint quadVAO = 0;
    GLuint quadVBO, quadEBO;

    GLuint depthMapFBO;
    glGenFramebuffers( 1 , &depthMapFBO );
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    GLuint depthMapTex;
    glGenTextures(1, &depthMapTex);
    glBindTexture(GL_TEXTURE_2D, depthMapTex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glm::vec4 zeros = glm::vec4( 1.0f, 0.0f, 0.0f, 0.0f );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &zeros.x);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "FBO pb" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLuint reflectionFBO, reflectionRBO, reflectionTex;
    glGenFramebuffers(1, &reflectionFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
    
    glGenTextures(1, &reflectionTex);
    glBindTexture(GL_TEXTURE_2D, reflectionTex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &zeros.x);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &reflectionRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, reflectionRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, reflectionRBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTex, 0);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "FBO pb" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    int framerate = 0;

    glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
    glClearDepth(1.0);
    
#if 1    
// Main loop
    while(!endgame)
    {
        current = SDL_GetTicks();
        deltaTime = (current - last)/1000;
        last = current;
        framerate = 1/deltaTime;
        framerate_t->setText(std::to_string(framerate));
        p->getController()->processEvents();
        p->move(deltaTime);

        //if ( m->isAlive() )
        // {
        //     m->checkAggro(p, deltaTime);
        // }

        //checkCollision( scene, p, deltaTime);
        test->setText(std::to_string(p->getLife()));
        

        glm::mat4 projection = glm::perspective(p->getCamera()->getZoom(),
                                                (float)screenWidth/(float)screenHeight,
                                                0.1f, 100.0f);
        glm::mat4 view = p->getCamera()->getViewMatrix();
        glm::mat4 eyeSpace = projection * view;
        
        depthShader->use();
        glUniformMatrix4fv(glGetUniformLocation(depthShader->getProgram(), "lightSpaceMatrix"),
                           1, GL_FALSE, glm::value_ptr(eyeSpace));


        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(2.5, 10);
        //glCullFace(GL_FRONT);
        drawScene( scene, depthShader, 0);
        glCullFace(GL_BACK);
        glDisable(GL_POLYGON_OFFSET_FILL);
  
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if(isGLError())
            cout << glGetError() << endl;

        
        glViewport(0, 0, screenWidth, screenHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        //glDisable(GL_CULL_FACE);
        drawScene(scene, depthShader, 0);
        if(isGLError())
            cout << "ERROR" << endl;
        
#if 1 
        lightningShader->use();
        // Transformation matrices        
        
        glUniformMatrix4fv(glGetUniformLocation(lightningShader->getProgram(), "projection"),
                           1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lightningShader->getProgram(), "view"),
                           1, GL_FALSE, glm::value_ptr(view));


        // Set the lighting uniforms
        glUniform3f(glGetUniformLocation(lightningShader->getProgram(), "viewPos"),
                    p->getCamera()->getPosition().x,p->getCamera()->getPosition().y, p->getCamera()->getPosition().z);
        glUniform3f(glGetUniformLocation(lightningShader->getProgram(), "lightPos"),
                    p->getCamera()->getPosition().x,p->getCamera()->getPosition().y, p->getCamera()->getPosition().z);

        // Point light 1
        glUniform3f(glGetUniformLocation(lightningShader->getProgram(), "pointLights[0].position"),
                    p->getCamera()->getPosition().x,
                    p->getCamera()->getPosition().y,
                    p->getCamera()->getPosition().z);     
        glUniform3f(glGetUniformLocation(lightningShader->getProgram(), "pointLights[0].ambient"),
                    0.05f, 0.05f, 0.05f);       
        glUniform3f(glGetUniformLocation(lightningShader->getProgram(), "pointLights[0].diffuse"),
                    1.0f, 1.0f, 1.0f); 
        glUniform3f(glGetUniformLocation(lightningShader->getProgram(), "pointLights[0].specular"),
                    1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(lightningShader->getProgram(), "pointLights[0].constant"),
                    1.0f);
        glUniform1f(glGetUniformLocation(lightningShader->getProgram(), "pointLights[0].linear"),
                    0.0005);   //0.7);
        glUniform1f(glGetUniformLocation(lightningShader->getProgram(), "pointLights[0].quadratic"),
                    0.00005);  // 1.8);  

        GLint maxTex;
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTex);
        glActiveTexture(GL_TEXTURE0 + --maxTex);       
        glUniform1i(glGetUniformLocation(lightningShader->getProgram(), "shadowMap"), maxTex); 
        glBindTexture(GL_TEXTURE_2D, depthMapTex);


        glm::mat4 remapping = glm::mat4(
            glm::vec4(0.5,0.0,0.0,0.5),
            glm::vec4(0.0,0.5,0.0,0.5),
            glm::vec4(0.0,0.0,0.5,0.5),
            glm::vec4(0.0,0.0,0.0,1.0)
                                        );
        glUniformMatrix4fv(glGetUniformLocation(waterShader->getProgram(), "remapping"),
                           1, GL_FALSE, glm::value_ptr(remapping));
            

        glm::mat4 invert = glm::mat4(
            glm::vec4(1.0,0.0,0.0,0.0),
            glm::vec4(0.0,-1.0,0.0,0.0),
            glm::vec4(0.0,0.0,1.0,0.0),
            glm::vec4(0.0,0.0,0.0,1.0)
                                     );
        glUniformMatrix4fv(glGetUniformLocation(lightningShader->getProgram(), "invert"),
                           1, GL_FALSE, glm::value_ptr(invert));
        
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
        glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
        glCullFace(GL_FRONT);
        drawScene(scene, lightningShader, 0);
        glCullFace(GL_BACK);
        
        invert = glm::mat4(1.0);
        glUniformMatrix4fv(glGetUniformLocation(lightningShader->getProgram(), "invert"),
                           1, GL_FALSE, glm::value_ptr(invert));
        
        glActiveTexture(GL_TEXTURE0 + --maxTex);       
        glUniform1i(glGetUniformLocation(waterShader->getProgram(), "reflection_texture"), maxTex); 
        glBindTexture(GL_TEXTURE_2D, reflectionTex);
        glViewport(0, 0, screenWidth, screenHeight);
        glBindFramebuffer( GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
        drawScene(scene, lightningShader, waterShader);

        
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        textShader->use();
        projection = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f);
        glUniformMatrix4fv(glGetUniformLocation(textShader->getProgram(), "projection"),
            1, GL_FALSE, glm::value_ptr(projection));
        test->draw(*textShader);
        framerate_t->draw(*textShader);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);


#if 1        
        GLuint indices[] = {  // Note that we start from 0!
        0, 1, 3, // First Triangle
            1, 2, 3  // Second Triangle
            };

        GLfloat quadVertices[] = {
        // Positions        // Texture Coords
        -1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
            -1.0f, 0.5f, 0.0f,    0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
            -0.5f,  1.0f, 0.0f,   0.0f, 0.0f, 0.0f,  1.0f, 1.0f,
            -0.5f, 0.5f, 0.0f,    0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
            };
        // Setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glGenBuffers(1, &quadEBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
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
       
        debugShader->use();
        glUniform1f(glGetUniformLocation(debugShader->getProgram(), "near_plane"), 0.1f );
        glUniform1f(glGetUniformLocation(debugShader->getProgram(), "far_plane"), 100.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, reflectionTex);
        
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
#endif  
        
#endif                
        // Actualisation de la fenêtre
        SDL_GL_SwapWindow(window);
    }
#endif
    // Deaalowing and cleaning app
    delete lightningShader;
    delete p;

    SDL_DestroyWindow(window);
    SDL_Quit();
    return(0);
}
