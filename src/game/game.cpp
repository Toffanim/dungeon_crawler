
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
    glEnable(GL_DEPTH_TEST);    // enable Z-buffering 
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
    glm::vec3 position = glm::vec3(ox, oy, 0.1f);
    modelMatrix = glm::translate(modelMatrix, position);    

    scene.push_back(
        new wall( mm.getModels()["wall"], modelMatrix ) );
    
    //plafond
    modelMatrix = glm::mat4();
    modelMatrix = glm::rotate(modelMatrix, -(PI/2.0f), glm::vec3(1.0f,0.0f,0.0f));
    position = glm::vec3(ox, -(oy+1)*1.0f, 0.9f);
    modelMatrix = glm::translate( modelMatrix, position );
    scene.push_back(
        new wall( mm.getModels()["wall"], modelMatrix) );

    if(r.murNord)
    {
    //mur sud
    modelMatrix = glm::mat4();
    modelMatrix = glm::rotate(modelMatrix, PI, glm::vec3(0.0f,1.0f,0.0f));
    position = glm::vec3(-(ox+1)*1.0f, -0.1f, (-oy));
    modelMatrix = glm::translate(modelMatrix, position);
    scene.push_back(
    new wall( mm.getModels()["wall"], modelMatrix) );
}
    //mur nord
    if(r.murSud)
    {
        
    modelMatrix = glm::mat4();
    position = glm::vec3(ox, -0.1f, (oy+1)*1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    scene.push_back(
    new wall( mm.getModels()["wall"], modelMatrix) );
    }
    if(r.murOuest)
    {
        //mur 
    modelMatrix = glm::mat4();
        modelMatrix = glm::rotate( modelMatrix, -(PI/2.0f), glm::vec3( 0.0f, 1.0f, 0.0f));
        position = glm::vec3(oy, -0.1f, -ox);
        modelMatrix = glm::translate( modelMatrix, position);
        scene.push_back(
    new wall( mm.getModels()["wall"], modelMatrix) );
    }
    if(r.murEst)
    {
        //mur ouest
        modelMatrix = glm::mat4();
        modelMatrix = glm::rotate( modelMatrix, (PI/2.0f), glm::vec3( 0.0f, 1.0f, 0.0f));
        position = glm::vec3(-(oy+1)*1.0f, -0.1f, (ox+1)*1.0f);
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
                //room
                //cout << "Room" << endl;
                //cout << idx << endl;
                //cout << img.data.size()<<endl;
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

void drawScene( vector<actor*>& scene, shader* shader)
{
    for ( vector<actor*>::iterator it = scene.begin();
    it != scene.end();
    ++it)
    {
        glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "model"),
                           1, GL_FALSE, glm::value_ptr((*it)->getModelMatrix()));
        (*it)->getModel()->Draw(*shader);
        
    }
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
    text* test = new text( "DEFAULT", glm::vec2(0.0,0.0));
    
    float last = 0.0f;
    float deltaTime = 0.0f;
    float current = 0.0f;
    #endif

    GLuint quadVAO = 0;
    GLuint quadVBO;

    GLuint depthMapFBO;
    glGenFramebuffers( 1 , &depthMapFBO );

    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#if 1    
// Main loop
    while(!endgame)
    {
        current = SDL_GetTicks();
        deltaTime = (current - last)/1000;
        last = current;
        p->getController()->processEvents();
        p->move(deltaTime);

        if ( m->isAlive() )
        {
            m->checkAggro(p, deltaTime);
        }

        checkCollision( scene, p, deltaTime);
        test->setText(std::to_string(p->getLife()));
        glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
        glClearDepth(1);        
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        
        glm::mat4 projection = glm::perspective(p->getCamera()->getZoom(), (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
        glm::mat4 view = p->getCamera()->getViewMatrix();
        glm::mat4 eyeSpace = projection * view;
        
        depthShader->use();
        glUniformMatrix4fv(glGetUniformLocation(depthShader->getProgram(), "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(eyeSpace));

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        drawScene( scene, depthShader );
        glBindFramebuffer(GL_FRAMEBUFFER, 0); 

        lightningShader->use();
        // Transformation matrices        
        glViewport(0, 0, screenWidth, screenHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
        glUniformMatrix4fv(glGetUniformLocation(lightningShader->getProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lightningShader->getProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));

        // Set the lighting uniforms
        glUniform3f(glGetUniformLocation(lightningShader->getProgram(), "viewPos"), p->getCamera()->getPosition().x,p->getCamera()->getPosition().y, p->getCamera()->getPosition().z);
        // Point light 1
        glUniform3f(glGetUniformLocation(lightningShader->getProgram(), "pointLights[0].position"), p->getCamera()->getPosition().x,p->getCamera()->getPosition().y, p->getCamera()->getPosition().z);     
        glUniform3f(glGetUniformLocation(lightningShader->getProgram(), "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);       
        glUniform3f(glGetUniformLocation(lightningShader->getProgram(), "pointLights[0].diffuse"), 1.0f, 1.0f, 1.0f); 
        glUniform3f(glGetUniformLocation(lightningShader->getProgram(), "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(lightningShader->getProgram(), "pointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightningShader->getProgram(), "pointLights[0].linear"), 0.0005);   //0.7);
        glUniform1f(glGetUniformLocation(lightningShader->getProgram(), "pointLights[0].quadratic"), 0.00005);  // 1.8);  

        GLint maxTex;
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTex);
        glActiveTexture(--maxTex);
        glUniform1i(glGetUniformLocation(lightningShader->getProgram(), "shadowMap"), maxTex); 
        glBindTexture(GL_TEXTURE_2D, depthMap);
        drawScene(scene, lightningShader);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        textShader->use();
        projection = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f);
        glUniformMatrix4fv(glGetUniformLocation(textShader->getProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        test->draw(*textShader);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        debugShader->use();
        glUniform1f(glGetUniformLocation(debugShader->getProgram(), "near_plane"), 0.1f );
        glUniform1f(glGetUniformLocation(debugShader->getProgram(), "far_plane"), 100.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        if (quadVAO == 0)
        {
            GLfloat quadVertices[] = {
                // Positions        // Texture Coords
                -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
                1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
                1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
            };
            // Setup plane VAO
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        }
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
        
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
