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
#include "../actor/actor.h"

class player
{
public:
    player(SDL_Keycode forwardKey = SDLK_z,
           SDL_Keycode backwardKey = SDLK_s,
           SDL_Keycode rightKey = SDLK_q,
           SDL_Keycode leftKey = SDLK_d);
    ~player();

    
    void mouseMoveCallback( int x, int y );
    void startMoveForward();
    void startMoveBackward();
    void startMoveRight();
    void startMoveLeft();
    void stopMoveForward();
    void stopMoveBackward();
    void stopMoveRight();
    void stopMoveLeft();
    void stopMoveUp();;
    void startMoveUp();
    void startMoveDown();
    void stopMoveDown();

    void move(float deltaTime);
    void updateCamera();
    void mouseMotion(int x, int y);

    bool hasKey();
    bool isAttacking( float deltaTime );

    void addLife( int value ) { life += value; }
    void addGold( int value ) { gold += value; }
    void addKey( int value ) { key += value; }
    int getGold() { return(gold);}
    int getLife() { return(life);}
    int getKey() {return(key);}
    
    std::map<SDL_Keycode, std::string> getMapping(){return(mapping);}
    controller* getController(){return(c);}
    camera* getCamera(){return(cam);}
    void setPosition( glm::vec3 p ) { position = p; updateCamera();}
    glm::vec3 getPosition(){return(position);}
    AABB& getAABB(){return(aabb);}

    bool getMoveLeft() { return(moveLeft);}
    bool getMoveRight() { return(moveRight); }
    bool getMoveForward() { return(moveForward); }
    bool getMoveBackward() { return(moveBackward); }

    int getAttack() { return(atk); }
private:
    SDL_Keycode forwardKey;
    SDL_Keycode backwardKey;
    SDL_Keycode rightKey;
    SDL_Keycode leftKey;

    bool moveLeft;
    bool moveRight;
    bool moveForward;
    bool moveBackward;
    bool moveUp;
    bool moveDown;
    bool firstTime;

    int life;
    int gold;
    int key;
    
    std::map<SDL_Keycode, std::string> mapping;
    float speed;
    glm::vec3 position;
    controller* c;
    camera* cam;

    float lastX;
    float lastY;

    float atkPerSec;
    int atk;
    float lastAttack;
    
    AABB aabb;
    void computeAABB();
};

#endif
