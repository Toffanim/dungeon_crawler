#if !defined(GAME_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define GAME_H

#include <map>
#include <utility>
#include <stdlib.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "../player/player.h"
#include "../controller/controller.h"
#include "../mesh/model.h"
#include "../camera/camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cfloat>
#include "../managers/modelManager.h"
#include "../actor/chest.h"
#include "../actor/wall.h"
#include "../text/text.h"
#include "../actor/door.h"
#include "../actor/monster.h"
#include "../actor/water.h"


struct room
{
    bool murSud;
    bool murEst;
    bool murOuest;
    bool murNord;
    bool ceil;
    bool floor;
    int offsetX;
    int offsetY;
};

class game
{    
public :    
    game(int width, int height);
    int init();
    void close();
    int mainLoop();
    void loadAssets();
    glm::vec2 loadMaze( const string &filename, vector<actor*>& scene, player* p);
    void drawScene( vector<actor*>& scene, shader* mainShader, shader* waterShader);
    void checkCollision(vector<actor*>& scene, player* p, float deltaTime);
    glm::vec2 loadWorld( const std::string path, vector<actor*>& scene, player* p);
    void checkMonsterAggro( vector<actor*>& scene, player* p, float deltaTime );
private :
   
    SDL_Window* window;
    SDL_GLContext openGLcontext;
    SDL_Event events;
    bool endgame;
    int screenWidth;
    int screenHeight;

    void startupGLDiagnostics();
    void makeRoom(vector<actor*>& scene, room& r, float depth, bool waterRoom);
    room& analyzeNeighbors( int idx, utils::ImageRGB& img, glm::vec3 color);
};

#endif
