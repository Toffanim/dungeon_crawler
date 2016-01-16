
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




void game::makeRoom(vector<actor*>& scene, room& r, float depth, bool waterRoom)
{
    modelManager& mm = modelManager::getInstance();

    int ox = r.offsetX;
    int oy = r.offsetY;

    //sol    

    glm::mat4 modelMatrix = glm::mat4();
    modelMatrix = glm::rotate(modelMatrix, (PI/2.0f), glm::vec3(1.0f,0.0f,0.0f));
    glm::vec3 position = glm::vec3(ox, oy, depth);
    modelMatrix = glm::translate(modelMatrix, position);    
    if ( !waterRoom)
    {
        scene.push_back(
            new wall( mm.getModels()["wall"], modelMatrix ) );
    }
    else
    {
        scene.push_back(
            new water( mm.getModels()["water"], modelMatrix ) );
    }

    if (r.ceil)
    {
        //plafond
        modelMatrix = glm::mat4();
        modelMatrix = glm::rotate(modelMatrix, -(PI/2.0f), glm::vec3(1.0f,0.0f,0.0f));
        position = glm::vec3(ox, -(oy+1)*1.0f, depth+1.0);
        modelMatrix = glm::translate( modelMatrix, position );
        scene.push_back(
            new wall( mm.getModels()["wall"], modelMatrix) );
    }
    if(r.murNord)
    {
        //mur sud
        modelMatrix = glm::mat4();
        modelMatrix = glm::rotate(modelMatrix, PI, glm::vec3(0.0f,1.0f,0.0f));
        position = glm::vec3(-(ox+1)*1.0f, -depth, (-oy));
        modelMatrix = glm::translate(modelMatrix, position);
        scene.push_back(
            new wall( mm.getModels()["wall"], modelMatrix) );
    }
    //mur nord
    if(r.murSud)
    {
        
        modelMatrix = glm::mat4();
        position = glm::vec3(ox, -depth, (oy+1)*1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        scene.push_back(
            new wall( mm.getModels()["wall"], modelMatrix) );
    }

    if(r.murOuest)
    {
        //mur 
        modelMatrix = glm::mat4();
        modelMatrix = glm::rotate( modelMatrix, (-PI/2.0f), glm::vec3( 0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate( modelMatrix, (PI), glm::vec3( 0.0f, 0.0f, 1.0f));
        position = glm::vec3(-(oy+1), depth-1.0, -ox);
        modelMatrix = glm::translate( modelMatrix, position);
        scene.push_back(
            new wall( mm.getModels()["wall"], modelMatrix) );
    }
    if(r.murEst)
    {
        //mur ouest
        modelMatrix = glm::mat4();
        modelMatrix = glm::rotate( modelMatrix, (PI/2.0f), glm::vec3( 0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate( modelMatrix, (-PI), glm::vec3( 0.0f, 0.0f, 1.0f));
        position = glm::vec3((oy)*1.0f, depth-1.0, (ox+1)*1.0f);
        modelMatrix = glm::translate( modelMatrix, position);
        scene.push_back(
            new wall( mm.getModels()["wall"], modelMatrix) );
    }
    


}

room& game::analyzeNeighbors( int idx, utils::ImageRGB& img, glm::vec3 color)
{
    //Normal room
    room r;
    r.murOuest = false;
    r.murEst = false;
    r.murNord = false;
    r.murSud = false;
    r.ceil = true;
    int nidx = idx-(img.w);
    if ( nidx < 0 || utils::isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                           color))
    {
        r.murNord = true;
    }
    nidx = idx+(img.w);
    if ( nidx >= img.data.size() || utils::isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                                          color)) 
    {
        r.murSud = true;
    }
    nidx = idx-1;
    if ( nidx < 0 || utils::isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                           color)) 
    {
        r.murOuest = true;
    }
    nidx = idx+1;
    if ( nidx >= img.data.size() || utils::isColorEquals( glm::vec3( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b ),
                                                          color)) 
    {
        r.murEst = true;
    }
    return r;
}

glm::vec2 game::loadMaze( const string &filename, vector<actor*>& scene, player* p)
{
    utils::ImageRGB img;
    utils::load_ppm(img, filename);
    int endPosX, endPosY;
    for( int i = 0;
         i < img.h;
         ++i)
    {
        for ( int j = 0;
              j < img.w;
              ++j )
        {
            int idx = (img.w *i)+j;
            if ( utils::isColorEquals( glm::vec3 ( img.data[idx].r, img.data[idx].g, img.data[idx].b),
                                       glm::vec3(255,255,255)))
            {
                //Normal room
                room r = analyzeNeighbors( idx, img, glm::vec3(0,0,0));
                r.offsetX = j;
                r.offsetY = i;
                makeRoom(scene,r, 0.0f, false);
            }

            if( utils::isColorEquals( glm::vec3 ( img.data[idx].r, img.data[idx].g, img.data[idx].b),
                                      glm::vec3(255,0,0)))
            {
                //Begining room
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
                //recul le player de moitier
                p->setPosition( p->getPosition() );

                room r = analyzeNeighbors(idx, img, glm::vec3(0,0,0));
                r.offsetX = j;
                r.offsetY = i;
                makeRoom(scene,r, 0.0f, false);
            }

            if ( utils::isColorEquals( glm::vec3( img.data[idx].r, img.data[idx].g, img.data[idx].b),
                                       glm::vec3(0,0,255)))
            {
                //WAter room
                room r = analyzeNeighbors( idx, img, glm::vec3(0,0,0));
                r.offsetX = j;
                r.offsetY = i;
                
                room r2 = analyzeNeighbors( idx, img, glm::vec3(0,0,255));
                r2.offsetX = j;
                r2.offsetY = i;
                r2.ceil = false;
                r2.murOuest = !r2.murOuest;
                r2.murEst = !r2.murEst;
                r2.murSud = !r2.murSud;
                r2.murNord = !r2.murNord;

                //NOTE(marc) : r2 is for under water
                makeRoom( scene, r, 0.0f, true);
                makeRoom( scene, r2, 1.0f, false);             
            }


            if ( utils::isColorEquals( glm::vec3( img.data[idx].r, img.data[idx].g, img.data[idx].b),
                                       glm::vec3(0,255,0)))
            {
                //END ROOM
                room r = analyzeNeighbors( idx, img, glm::vec3(0,0,0));
                r.offsetX = j;
                r.offsetY = i;
                endPosX = j;
                endPosY = i;
                makeRoom( scene, r, 0.0f,false);
            }


            if ( utils::isColorEquals( glm::vec3 ( img.data[idx].r, img.data[idx].g, img.data[idx].b),
                                       glm::vec3(255,126,0)))
            {
                //door
                //NOTE(marc): we should check if door is valid or on image border
                //to be more accurate.
                room r = analyzeNeighbors(idx, img, glm::vec3(0,0,0));
                r.offsetX = j;
                r.offsetY = i;
                makeRoom(scene,r, 0.0f, false);
                int nidx = idx-1;
                int nidx2 = idx+1;
                if(  utils::isColorEquals( glm::vec3 ( img.data[nidx].r, img.data[nidx].g, img.data[nidx].b),
                                           glm::vec3(0,0,0))
                     &&  utils::isColorEquals( glm::vec3 ( img.data[nidx2].r, img.data[nidx2].g, img.data[nidx2].b),
                                               glm::vec3(0,0,0)))
                {
//door along x axis
                    
                    modelManager& mm = modelManager::getInstance();
                    glm::vec3 position = glm::vec3(j, 0.0f, (i+0.5f)*1.0f);
                    glm::mat4 modelMatrix = glm::mat4();
                    modelMatrix = glm::translate(modelMatrix, position);
                    
                    scene.push_back(
                        new door( mm.getModels()["door"], modelMatrix )); 
                }
                else
                {
                    //door along y axis
                   
                    modelManager& mm = modelManager::getInstance();
                    glm::mat4 modelMatrix = glm::mat4();
                    modelMatrix = glm::rotate( modelMatrix, (PI/2.0f), glm::vec3( 0.0f, 1.0f, 0.0f));         
                    glm::vec3 position = glm::vec3(-(i+1)*1.0f, 0.0f, (j+0.5)*1.0f);
                    modelMatrix = glm::translate( modelMatrix, position);
                    scene.push_back(
                        new door( mm.getModels()["door"], modelMatrix) );
                }
                
            }
        }
    }
    return glm::vec2(endPosX, endPosY);
}

void game::drawScene( vector<actor*>& scene, shader* mainShader, shader* waterShader)
{
    //Render everything excpet water
    mainShader->use();
    for ( vector<actor*>::iterator it = scene.begin();
          it != scene.end();
          ++it)
    {
        //TODO(marc) : make instancied rendering for walls ?
        if((*it)->getType() != "water")
        {
            glUniformMatrix4fv(glGetUniformLocation(mainShader->getProgram(), "model"),
                               1, GL_FALSE, glm::value_ptr((*it)->getModelMatrix()));
            (*it)->Draw(mainShader);
        }
    }
    //NOTE(marc): Besoin de render l'eau en dernier pour les problèmes de blending qui ne sont pas
    //order independent, pas le temps de faire un blending non order independent
    //Render only water
    if(waterShader)
    {
        waterShader->use();
        for ( vector<actor*>::iterator it = scene.begin();
              it != scene.end();
              ++it)
        {
            //TODO(marc) : make instancied rendering for walls ?
            if((*it)->getType() == "water")
            {
                glUniformMatrix4fv(glGetUniformLocation(waterShader->getProgram(), "model"),
                                   1, GL_FALSE, glm::value_ptr((*it)->getModelMatrix()));
                (*it)->Draw(waterShader);       
            }
        
        }
    }
    
}

bool isGLError()
{
    int error = glGetError();
    if( error != GL_NO_ERROR )
        return error;
    return false;
}

void game::checkCollision(vector<actor*>& scene, player* p, float deltaTime)
{
    for( vector<actor*>::iterator it = scene.begin();
         it != scene.end();
         ++it)
    {
        if ( (*it)->getType() == "water" )
        {
            AABB aabb = (*it)->getAABB();
            aabb.max.y += 1.0;
            aabb.min.y += -1.0;
            if ( utils::AABBtoAABB( aabb, p->getAABB()))
            {
                (*it)->doCollision(p, deltaTime);
            }
        }
        else
        {
            if ( utils::AABBtoAABB( (*it)->getAABB(), p->getAABB()))
            {
                (*it)->doCollision(p, deltaTime);
            }
        }
    }
}

void game::loadAssets()
{
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
    manager.getModels().insert(
        std::pair<std::string, Model*>
        ("monster2", new Model( "../Assets/Models/Alien_Necromorph/Alien_Necromorph.obj" )));
}

glm::vec2 game::loadWorld( const std::string path, vector<actor*>& scene, player* p)
{
    modelManager& manager = modelManager::getInstance();
    ifstream file(path);
    string line;
    int lineNb = 0;
    int chestNb = 0;
    int monsterNb = 0;
    if(file.is_open())
    {
        getline (file,line);
        getline (file, line);
        glm::vec2 endPos = loadMaze( line, scene, p);
        getline(file, line);
        chestNb = std::stoi(line, nullptr);
        for ( int i = 0;
              i < chestNb;
              ++i)
        {
            getline(file, line);
            stringstream ss(line);
            string item;
            vector<string> elements;
            while (getline(ss, item, ':'))
            {
                elements.push_back(item);
            }
            glm::mat4 modelMatrix = glm::mat4();
            
            modelMatrix = glm::translate( modelMatrix,
                                          glm::vec3( stof(elements[1], nullptr)+0.5,
                                                     0.0,
                                                     stof(elements[2], nullptr)+0.5));
            modelMatrix = glm::scale( modelMatrix, glm::vec3(0.1,0.1,0.1));
            chest* aChest;
            if (elements[4] == "gold")
                aChest = new chest( manager.getModels()[elements[6]], modelMatrix, chest::Type::GOLD, stoi(elements[5], nullptr) );
            else if ( elements[4] == "life")
                aChest = new chest( manager.getModels()[elements[6]], modelMatrix, chest::Type::LIFE, stoi(elements[5], nullptr) );
            else
                aChest = new chest( manager.getModels()[elements[6]], modelMatrix, chest::Type::KEY, stoi(elements[5], nullptr) );
            scene.push_back(aChest);
        }
        getline(file, line);
        monsterNb = stoi(line, nullptr);
        for ( int i = 0;
              i < monsterNb;
              ++i)
        {
            getline(file, line);
            stringstream ss(line);
            string item;
            vector<string> elements;
            while (getline(ss, item, ':'))
            {
                elements.push_back(item);
            }
            glm::mat4 modelMatrix = glm::mat4();
            modelMatrix = glm::translate( modelMatrix,
                                          glm::vec3( stof(elements[1], nullptr)+0.5,
                                                     0.0,
                                                     stof(elements[2], nullptr)+0.5));
            modelMatrix = glm::scale( modelMatrix, glm::vec3(0.1,0.1,0.1));
            monster* m;
            if ( elements[4] == "gold" )
            {
                m = new monster( manager.getModels()[elements[9]],
                                 modelMatrix,
                                 glm::vec3( stof(elements[1], nullptr)+0.5,
                                            0.0f,
                                            stof(elements[2], nullptr)+0.5),
                                 stoi(elements[6], nullptr),
                                 stof(elements[7], nullptr),
                                 stoi(elements[8], nullptr),
                                 2.0,
                                 monster::Type::GOLD,
                                 stoi(elements[5], nullptr)
                                 );
            }
            else
            {
                m = new monster( manager.getModels()[elements[9]],
                                 modelMatrix,
                                 glm::vec3( stof(elements[1], nullptr)+0.5,
                                            0.0f,
                                            stof(elements[2], nullptr)+0.5),
                                 stoi(elements[6], nullptr),
                                 stof(elements[7], nullptr),
                                 stoi(elements[8], nullptr),
                                 2.0,
                                 monster::Type::LIFE,
                                 stoi(elements[5], nullptr)
                                 );
            }
            scene.push_back(m);
        }
        file.close();
        return endPos;
    }
    else
    {
        cout << "file problem" << endl;
        return glm::vec2(0.0);
    }
    
}


void game::checkMonsterAggro( vector<actor*>& scene, player* p, float deltaTime )
{
    for ( vector<actor*>::iterator it = scene.begin();
          it != scene.end();
          ++it)
    {
        if( (*it)->getType() == "monster" )
        {
            monster* m = static_cast<monster*>( (*it) );
            if ( m->isAlive() )
            {
                m->checkAggro(p, deltaTime);
            }
        }
    }
}

int game::mainLoop()
{
    vector<actor*> scene;
    player* p = new player();
    p->getController()->setMiscCallback(SDL_WINDOWEVENT_CLOSE, std::bind(&game::close, this));
    p->getController()->setKeyPressCallback(SDLK_ESCAPE, std::bind(&game::close, this));
    loadAssets();
    glm::vec2 endPos = loadWorld( "../Assets/toLoad.txt", scene, p);
    AABB endAABB;
    endAABB.min.x = endPos.x;
    endAABB.max.x = endPos.x +1;
    endAABB.min.z = endPos.y;
    endAABB.max.z = endPos.y +1;
    endAABB.min.y = 0;
    endAABB.max.y = 1;
    //Load shaders
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
    shader* blurShader = new shader( "../Shaders/blur" );
    blurShader->init();
    


    text* life = new text( "DEFAULT", glm::vec2(0.0,0.0), 50, false);
    //NOTE(marc) : rajouter un sens au texte pour pouvoir mettre la position
    //dans les coins a droite par exemple
    //Rajouter aussi une taille au texte (changement au moment du loadfont)
    //peut etre pouvoir donner une font et une couleur aussi, peut etre pas util pourn le moment
    text* framerate_t = new text( "DEFAULT", glm::vec2(700.0, 0.0), 10, true);
    text* gold = new text( "DEFAULT", glm::vec2(0.0, 50.0), 50, false);
    text* keys = new text( "DEFAULT", glm::vec2(0.0, 100.0), 50, false);
    
    float last = 0.0f;
    float deltaTime = 0.0f;
    float current = 0.0f;


    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    GLuint quadVAO = 0;
    GLuint quadVBO, quadEBO;

    //Create depthmap FBO
    GLuint depthMapFBO, depthMapTex;
    glGenFramebuffers( 1 , &depthMapFBO );
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glGenTextures(1, &depthMapTex);
    glBindTexture(GL_TEXTURE_2D, depthMapTex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glm::vec4 zeros = glm::vec4( 0.0f, 1.0f, 0.0f, 0.0f );
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

    //Create reflection FBO
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

    //Create blur FBOs
    GLuint pingpongFBO[2];
    GLuint pingpongBuffer[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongBuffer);
    for (GLuint i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL
                     );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0
                               );
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            cout << "FBO pb" << endl;
    }


    int framerate = 0;

    glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
    glClearDepth(1.0);

    bool gameOver = false;
    bool winGame = false;
       
// Main loop
    while(!endgame)
    {
        if ( !gameOver && !winGame )
        {
        current = SDL_GetTicks();
        deltaTime = (current - last)/1000;
        last = current;
        framerate = 1/deltaTime;
        framerate_t->setText(std::to_string(framerate));
        p->getController()->processEvents();
        p->move(deltaTime);

        if ( utils::AABBtoAABB(endAABB, p->getAABB()))
        {
            winGame = true;
        }

        
        life->setText("LIFE :" + std::to_string(p->getLife()));
        gold->setText("GOLD :" + std::to_string(p->getGold()));
        keys->setText("KEYS :" + std::to_string(p->getKey()));

        checkMonsterAggro( scene, p, deltaTime );
        checkCollision( scene, p, deltaTime);
        if (p->getLife() <= 0 )
        {
            gameOver = true;
        }

        glm::mat4 projection = glm::perspective(p->getCamera()->getZoom(),
                                                (float)screenWidth/(float)screenHeight,
                                                0.1f, 100.0f);
        glm::mat4 view = p->getCamera()->getViewMatrix();
        glm::mat4 te = glm::mat4(
            glm::vec4( 0.0, 0.0, 0.0, 0.0),
            glm::vec4( 0.0, 0.0, 0.0, -2.0),
            glm::vec4( 0.0, 0.0, 0.0, 0.0),
            glm::vec4( 0.0, 0.0, 0.0, 0.0)
                                 );
        glm::mat4 rotView = p->getCamera()->getRotatedViewMatrix();
        glm::mat4 eyeSpace = projection * (view + te);
        
        depthShader->use();
        glUniformMatrix4fv(glGetUniformLocation(depthShader->getProgram(), "lightSpaceMatrix"),
                           1, GL_FALSE, glm::value_ptr(eyeSpace));


        //RENDER SHADOW MAP
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(2.5, 10);
        glCullFace(GL_FRONT);
        drawScene( scene, depthShader, 0);
        glCullFace(GL_BACK);
        glDisable(GL_POLYGON_OFFSET_FILL);
  
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if(isGLError())
            cout << glGetError() << endl;       

//SET WATER SHADER UNIFROMS
        waterShader->use();
// Transformation matrices                
        glUniformMatrix4fv(glGetUniformLocation(waterShader->getProgram(), "projection"),
                           1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(waterShader->getProgram(), "view"),
                           1, GL_FALSE, glm::value_ptr(view));

// Set the lighting uniforms
        glUniform3f(glGetUniformLocation(waterShader->getProgram(), "viewPos"),
                    p->getCamera()->getPosition().x,p->getCamera()->getPosition().y, p->getCamera()->getPosition().z);
        glUniform3f(glGetUniformLocation(waterShader->getProgram(), "lightPos"),
                    p->getCamera()->getPosition().x,p->getCamera()->getPosition().y, p->getCamera()->getPosition().z);

// Point light 1
        glUniform3f(glGetUniformLocation(waterShader->getProgram(), "pointLights[0].position"),
                    p->getCamera()->getPosition().x,
                    p->getCamera()->getPosition().y,
                    p->getCamera()->getPosition().z);     
        glUniform3f(glGetUniformLocation(waterShader->getProgram(), "pointLights[0].ambient"),
                    0.05f, 0.05f, 0.05f);       
        glUniform3f(glGetUniformLocation(waterShader->getProgram(), "pointLights[0].diffuse"),
                    1.0f, 1.0f, 1.0f); 
        glUniform3f(glGetUniformLocation(waterShader->getProgram(), "pointLights[0].specular"),
                    1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(waterShader->getProgram(), "pointLights[0].constant"),
                    1.0f);
        glUniform1f(glGetUniformLocation(waterShader->getProgram(), "pointLights[0].linear"),
                    0.7); //0.0005);   //0.7);
        glUniform1f(glGetUniformLocation(waterShader->getProgram(), "pointLights[0].quadratic"),
                    1.8); //0.00005);  // 1.8)

//SET LIGHTNING SHADER UNIFORMS
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
                    0.22); //0.0005);   //0.7);
        glUniform1f(glGetUniformLocation(lightningShader->getProgram(), "pointLights[0].quadratic"),
                    0.20); //0.00005);  // 1.8);  

        GLint maxTex;
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTex);
        glActiveTexture(GL_TEXTURE0 + --maxTex);       
        glUniform1i(glGetUniformLocation(lightningShader->getProgram(), "shadowMap"), maxTex);
        glBindTexture(GL_TEXTURE_2D, depthMapTex);          


//RENDER WATER REFLECTION
        glUniformMatrix4fv(glGetUniformLocation(lightningShader->getProgram(), "view"),
                           1, GL_FALSE, glm::value_ptr(rotView));
        glm::vec4 plane = glm::vec4( 0.0 , 1.0 , 0.0, 0.0 );
        glUniform4f(glGetUniformLocation(lightningShader->getProgram(), "clipPlane"),
                    plane.x, plane.y, plane.z, plane.w);   

        glm::mat4 invert = glm::mat4 (
            glm::vec4( 1.0, 0.0, 0.0, 0.0),
            glm::vec4( 0.0, -1.0, 0.0, 0.0),
            glm::vec4( 0.0, 0.0, 1.0, 0.0),
            glm::vec4( 0.0, 0.0, 0.0, 1.0));
        glUniformMatrix4fv(glGetUniformLocation(lightningShader->getProgram(), "invert"),
                           1, GL_FALSE, glm::value_ptr(invert));
       
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
        glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
        glCullFace(GL_FRONT);
        glEnable(GL_CLIP_DISTANCE0);
        drawScene(scene, lightningShader, 0);
        glCullFace(GL_BACK);
        invert = glm::mat4(1.0);
        glUniformMatrix4fv(glGetUniformLocation(lightningShader->getProgram(), "invert"),
                           1, GL_FALSE, glm::value_ptr(invert));


//BLUR REFLECTION
        GLboolean horizontal = true, first_iteration = true;
        GLuint amount = 20;
        GLuint _quadVBO, _quadVAO, _quadEBO;
        blurShader->use();
        glActiveTexture(GL_TEXTURE0);
        glViewport(0, 0, screenWidth, screenHeight);
        for (GLuint i = 0; i < amount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]); 
            glUniform1i(glGetUniformLocation(blurShader->getProgram(), "horizontal"), horizontal);
            glBindTexture(
                GL_TEXTURE_2D, first_iteration ? reflectionTex : pingpongBuffer[!horizontal]  ); 
            GLuint indices[] = {  // Note that we start from 0!
                0, 1, 3, // First Triangle
                1, 2, 3  // Second Triangle
            };

            GLfloat quadVertices[] = {
// Positions                                 // Texture Coords
                -1.0f,  1.0f, 0.0f,   0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f,    0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
                1.0f,  1.0f, 0.0f,   0.0f, 0.0f, 0.0f,  1.0f, 1.0f,
                1.0f, -1.0f, 0.0f,    0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
            };
// Setup plane VAO
            glGenVertexArrays(1, &_quadVAO);
            glGenBuffers(1, &_quadVBO);
            glGenBuffers(1, &_quadEBO);
            glBindVertexArray(_quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, _quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _quadEBO);
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

            glBindVertexArray(_quadVAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            glBindVertexArray(0); // Unbind VAO
        
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        lightningShader->use();
        glUniformMatrix4fv(glGetUniformLocation(lightningShader->getProgram(), "view"),
                           1, GL_FALSE, glm::value_ptr(view));
        plane = glm::vec4( 0.0 , 1.0 , 0.0, 200.0 );
        glUniform4f(glGetUniformLocation(lightningShader->getProgram(), "clipPlane"),
                    plane.x, plane.y, plane.z, plane.w);
        glUniform4f(glGetUniformLocation(waterShader->getProgram(), "clipPlane"),
                    plane.x, plane.y, plane.z, plane.w);

        waterShader->use();
        glActiveTexture(GL_TEXTURE0 + --maxTex);       
        glUniform1i(glGetUniformLocation(waterShader->getProgram(), "reflection_texture"), maxTex); 
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal]);
        lightningShader->use();

//RENDER SCENE
        glViewport(0, 0, screenWidth, screenHeight);
        glBindFramebuffer( GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawScene(scene, lightningShader, waterShader);

//RENDER TEXT
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        textShader->use();
        projection = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f);
        glUniformMatrix4fv(glGetUniformLocation(textShader->getProgram(), "projection"),
                           1, GL_FALSE, glm::value_ptr(projection));
        life->draw(*textShader);
        framerate_t->draw(*textShader);
        gold->draw(*textShader);
        keys->draw(*textShader);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

#if 0
        //TODO(marc) : faire une fonction pour render un quad et éviter de dupliquer le code
//DEBUG DISPLAY
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
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal] );
        
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
#endif  
        
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            p->getController()->processEvents();
            if ( gameOver )
            {
                life->setText("YOU LOSE !! Press Esc to close");
            }
            if ( winGame )
            {
                life->setText("YOU WIN !! Press Esc to close");
            }
            glViewport(0, 0, screenWidth, screenHeight);
            glBindFramebuffer( GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            textShader->use();
            glm::mat4 projection = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f);
            glUniformMatrix4fv(glGetUniformLocation(textShader->getProgram(), "projection"),
                               1, GL_FALSE, glm::value_ptr(projection));
            life->draw(*textShader);
        }
// Actualisation de la fenêtre
        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return(0);
}
