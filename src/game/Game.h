#if !defined(GAME_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice: $
   ======================================================================== */

#define GAME_H

#include <map>
#include <utility>
#include <stdlib.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "../player/Player.h"
#include "../controller/Controller.h"
#include "../mesh/Model.h"
#include "../camera/Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cfloat>
#include "../managers/ModelManager.h"
#include "../actor/Chest.h"
#include "../actor/Wall.h"
#include "../text/Text.h"
#include "../actor/Door.h"
#include "../actor/Monster.h"
#include "../actor/Water.h"


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

class Game
{    
public :    
    Game(int width, int height);
    int init();
    void close();
    int mainLoop();
    void loadAssets();
    glm::vec2 loadMaze( const std::string &filename, std::vector<Actor*>& scene, Player* p);
    void drawScene( std::vector<Actor*>& scene, Shader* mainShader, Shader* waterShader);
    void checkCollision(std::vector<Actor*>& scene, Player* p, float deltaTime);
    glm::vec2 loadWorld( const std::string path, std::vector<Actor*>& scene, Player* p);
    void checkMonsterAggro( std::vector<Actor*>& scene, Player* p, float deltaTime );
private :
   
    SDL_Window* window;
    SDL_GLContext openGLcontext;
    SDL_Event events;
    bool endgame;
    int screenWidth;
    int screenHeight;
    
    bool isGLError();
    void startupGLDiagnostics();
    void makeRoom(std::vector<Actor*>& scene, room& r, float depth, bool waterRoom);
    room& analyzeNeighbors( int idx, Utils::ImageRGB& img, glm::vec3 color);
};

#endif
