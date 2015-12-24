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
    glEnable(GL_DEPTH_TEST);    // enable Z-buffering 
    glEnable(GL_CULL_FACE);
    
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

void makeRoom(vector<SceneNode>& scene, room& r)
{
    SceneNode node;

    // if (wallNorth) ...
    // coord : 1.0 = 1m
    //scal = tileSize

    int ox = r.offsetX;
    int oy = r.offsetY;
    //sol
    node.model = new Model("../Assets/Models/Wall/wall.obj");
    node.modelMatrix = glm::rotate(node.modelMatrix, (PI/2.0f), glm::vec3(1.0f,0.0f,0.0f));
    glm::vec3 position = glm::vec3(ox, oy, 0.1f);
    node.modelMatrix = glm::translate(node.modelMatrix, position);
    scene.push_back(node);

    //plafond
    SceneNode n;
    n.model = node.model;
    n.modelMatrix = glm::rotate(n.modelMatrix, -(PI/2.0f), glm::vec3(1.0f,0.0f,0.0f));
    position = glm::vec3(ox, -(oy+1)*1.0f, 0.9f);
    n.modelMatrix = glm::translate( n.modelMatrix, position );
    scene.push_back(n);

    if(r.murNord)
    {
     //mur sud
    SceneNode node2;
    node2.model = node.model;
    node2.modelMatrix = glm::rotate(node2.modelMatrix, PI, glm::vec3(0.0f,1.0f,0.0f));
    position = glm::vec3(-(ox+1)*1.0f, -0.1f, (-oy));
    node2.modelMatrix = glm::translate(node2.modelMatrix, position);
    scene.push_back(node2);
    }
    //mur nord
    if(r.murSud)
    {
        SceneNode node3;
        node3.model = node.model;
        position = glm::vec3(ox, -0.1f, (oy+1)*1.0f);
        node3.modelMatrix = glm::translate(node3.modelMatrix, position);
        scene.push_back(node3);
    }
    if(r.murOuest)
    {
        //mur est
        SceneNode node4;
        node4.model = node.model;
        node4.modelMatrix = glm::rotate( node4.modelMatrix, -(PI/2.0f), glm::vec3( 0.0f, 1.0f, 0.0f));
        position = glm::vec3(oy, -0.1f, -ox);
        node4.modelMatrix = glm::translate( node4.modelMatrix, position);
        scene.push_back(node4);
    }
    if(r.murEst)
    {
        //mur ouest
        SceneNode node5;
        node5.model = node.model;
        node5.modelMatrix = glm::rotate( node5.modelMatrix, (PI/2.0f), glm::vec3( 0.0f, 1.0f, 0.0f));
        position = glm::vec3(-(oy+1)*1.0f, -0.1f, (ox+1)*1.0f);
    node5.modelMatrix = glm::translate( node5.modelMatrix, position);
    scene.push_back(node5);
    }
}

bool isColorEquals( glm::vec3 c1, glm::vec3 c2)
{
    if ( c1.x == c2.x && c1.y == c2.y && c1.z == c2.z)
        return (true);
    return (false);
}

void loadMaze( const string &filename, vector<SceneNode>& scene, player* p)
{
    ImageRGB img;
    load_ppm(img, filename);
    room r;
    r.offsetX = 2;
    r.offsetY = 0;
    r.murOuest = true;
    r.murEst = false;
    r.murNord = true;
    r.murSud = false;
    // makeRoom(scene, r);
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
                p->setPosition( p->getPosition()- p->getCamera()->getFront());
            }
            if ( isColorEquals( glm::vec3 ( img.data[idx].r, img.data[idx].g, img.data[idx].b),
                                glm::vec3(255,126,0)))
            {
                //door
                //NOTE(marc): we should check if door is valid or on image border
                //to be more accurate.
                int nidx = idx-1;
                int nidx2 = idx+1;
                if(  isColorEquals( glm::vec3 ( img.data[idx].r, img.data[idx].g, img.data[idx].b),
                                    glm::vec3(255,126,0))
                     &&  isColorEquals( glm::vec3 ( img.data[idx].r, img.data[idx].g, img.data[idx].b),
                                        glm::vec3(255,126,0)))
                {
//door along x axis
                    cout << "door x axis" << endl;                    
                    SceneNode node3;
                    node3.model = new Model("../Assets/Models/Wall/wall.obj");
                    glm::vec3 position = glm::vec3(j, -0.1f, (i+0.5f)*1.0f);
                    node3.modelMatrix = glm::translate(node3.modelMatrix, position);
                    scene.push_back(node3);
                }
                else
                {
//door along y axis
                    cout << "door y axis" << endl;
                }
           
            }
        }        
        //cout << (int)(*it).r << " | " << (int)(*it).g << " | " << (int)(*it).b << endl;
    }

}

void drawScene( vector<SceneNode>& scene, shader* shader)
{
    for ( vector<SceneNode>::iterator it = scene.begin();
    it != scene.end();
    ++it)
    {
        glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), "model"),
        1, GL_FALSE, glm::value_ptr((*it).modelMatrix));
        (*it).model->Draw(*shader);
    }
}

int game::mainLoop()
{
    vector<SceneNode> scene;
    player* p = new player();
    p->getController()->setMiscCallback(SDL_WINDOWEVENT_CLOSE, std::bind(&game::close, this));
    p->getController()->setKeyPressCallback(SDLK_ESCAPE, std::bind(&game::close, this));
    loadMaze( "../Assets/mazeTest.ppm", scene, p);

    SceneNode chestSN;
    chestSN.model = new Model( "../Assets/Models/Chest/treasure_chest.obj" );
    chestSN.modelMatrix = glm::translate( chestSN.modelMatrix, glm::vec3(p->getPosition().x, -0.1f, p->getPosition().z));
    chestSN.modelMatrix = glm::scale( chestSN.modelMatrix, glm::vec3(0.1,0.1,0.1));
    scene.push_back(chestSN);

    chest* aChest = new chest( chestSN.model, chestSN.modelMatrix, chest::Type::GOLD, 100 );
    
    modelManager& manager = modelManager::getInstance();
    manager.getModels().push_back( new Model( "../Assets/Models/Chest/treasure_chest.obj" ) );
    manager.getModels().push_back( new Model( "../Assets/Models/Wall/wall.obj" ));

    shader* s = new shader("../Shaders/simpleLight" );
    
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

        glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
        glClearDepth(1);        
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        
        s->init();
        s->use();
        // Transformation matrices
        glm::mat4 projection = glm::perspective(p->getCamera()->getZoom(), (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
        glm::mat4 view = p->getCamera()->getViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(s->getProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(s->getProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));

        // Set the lighting uniforms
        glUniform3f(glGetUniformLocation(s->getProgram(), "viewPos"), p->getCamera()->getPosition().x,p->getCamera()->getPosition().y, p->getCamera()->getPosition().z);
        // Point light 1
        glUniform3f(glGetUniformLocation(s->getProgram(), "pointLights[0].position"), p->getCamera()->getPosition().x,p->getCamera()->getPosition().y, p->getCamera()->getPosition().z);     
        glUniform3f(glGetUniformLocation(s->getProgram(), "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);       
        glUniform3f(glGetUniformLocation(s->getProgram(), "pointLights[0].diffuse"), 1.0f, 1.0f, 1.0f); 
        glUniform3f(glGetUniformLocation(s->getProgram(), "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(s->getProgram(), "pointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(s->getProgram(), "pointLights[0].linear"), 0.0005);   //0.7);
        glUniform1f(glGetUniformLocation(s->getProgram(), "pointLights[0].quadratic"), 0.00005);  // 1.8);  

        drawScene(scene, s);               
        // Actualisation de la fenêtre
        SDL_GL_SwapWindow(window);
    }

    // Deaalowing and cleaning app
    delete s;
    delete p;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return(0);
}
