#if !defined(PLAYER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define PLAYER_H

#include "Model.h"
#include "camera.h"
#include "controller.h"
#include <SDL2/SDL.h>
#include <map>

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

    std::map<SDL_Keycode, std::string> getMapping(){return(mapping);}
    controller* getController(){return(c);}
private:
    SDL_Keycode forwardKey;
    SDL_Keycode backwardKey;
    SDL_Keycode rightKey;
    SDL_Keycode leftKey;

    bool moveLeft;
    bool moveRight;
    bool moveForward;
    bool moveBackward;

    std::map<SDL_Keycode, std::string> mapping;
    float speed;
    controller* c;
    camera* cam;
    Model* model;
};

#endif
