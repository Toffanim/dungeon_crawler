#if !defined(PLAYER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define PLAYER_H

#include "../mesh/model.h"
#include "../camera/camera.h"
#include "../controller/controller.h"
#include <SDL2/SDL.h>
#include <map>
#include <glm/glm.hpp>

class player
{
public:
    player(SDL_Keycode forwardKey = SDLK_z,
           SDL_Keycode backwardKey = SDLK_s,
           SDL_Keycode rightKey = SDLK_q,
           SDL_Keycode leftKey = SDLK_d);

    void mouseMoveCallback( int x, int y );
    void startMoveForward();
    void startMoveBackward();
    void startMoveRight();
    void startMoveLeft();
    void stopMoveForward();
    void stopMoveBackward();
    void stopMoveRight();
    void stopMoveLeft();

    void move(float deltaTime);
    void updateCamera();
    void mouseMotion(int x, int y);

    std::map<SDL_Keycode, std::string> getMapping(){return(mapping);}
    controller* getController(){return(c);}
    camera* getCamera(){return(cam);}
private:
    SDL_Keycode forwardKey;
    SDL_Keycode backwardKey;
    SDL_Keycode rightKey;
    SDL_Keycode leftKey;

    bool moveLeft;
    bool moveRight;
    bool moveForward;
    bool moveBackward;
    bool firstTime;
    
    std::map<SDL_Keycode, std::string> mapping;
    float speed;
    glm::vec3 position;
    controller* c;
    camera* cam;
    Model* model;

    float lastX;
    float lastY;
};

#endif
