/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "player.h"

player::player(SDL_Keycode forwardKey, SDL_Keycode backwardKey, SDL_Keycode rightKey, SDL_Keycode leftKey) :
        forwardKey(forwardKey), backwardKey(backwardKey), rightKey(rightKey), leftKey(leftKey),
        moveForward(false), moveRight(false), moveLeft(false), moveBackward(false),
        speed(1.5f), position( glm::vec3( 0.0f, 0.0f, 0.0f )),
        lastX(0.0f), lastY(0.0f)
{
    model = new Model( "Alien_Necromorph/Alien_Necromorph.obj" );
    
    mapping.insert( std::pair<SDL_Keycode,std::string>(forwardKey, "moveForward"));
        mapping.insert( std::pair<SDL_Keycode,std::string>(backwardKey, "moveBackward"));
        mapping.insert( std::pair<SDL_Keycode,std::string>(rightKey, "moveRight"));
        mapping.insert( std::pair<SDL_Keycode,std::string>(leftKey, "moveLeft"));

        c = new controller();
        c->setKeyPressCallback(forwardKey, std::bind(&player::startMoveForward, this));
        c->setKeyPressCallback(backwardKey, std::bind(&player::startMoveBackward, this));
        c->setKeyPressCallback(rightKey, std::bind(&player::startMoveRight, this));
        c->setKeyPressCallback(leftKey, std::bind(&player::startMoveLeft, this));
        c->setKeyReleaseCallback(forwardKey, std::bind(&player::stopMoveForward, this));
        c->setKeyReleaseCallback(backwardKey, std::bind(&player::stopMoveBackward, this));
        c->setKeyReleaseCallback(rightKey, std::bind(&player::stopMoveRight, this));
        c->setKeyReleaseCallback(leftKey, std::bind(&player::stopMoveLeft, this));
        std::function<void(int, int)> f = std::bind(&player::mouseMotion, this, std::placeholders::_1, std::placeholders::_2);
        c->setMouseCallback(f);
        cam = new camera( position );
}


void player::stopMoveForward()
{
    moveForward = false;
}

void player::stopMoveBackward()
{
    moveBackward = false;
}

void player::stopMoveRight()
{
    moveRight = false;
}

void player::stopMoveLeft()
{
    moveLeft = false;
}


void player::startMoveForward()
{
    moveForward = true;
}

void player::startMoveBackward()
{
    moveBackward = true;
}

void player::startMoveRight()
{
    moveRight = true;
}

void player::startMoveLeft()
{
    moveLeft = true;
}

void player::move(float deltaTime)
{
    glm::vec3 pos = position;
    float x = pos.x;
    float y = pos.y;
    float z = pos.z;
    if(moveForward)
    {
        z += speed*deltaTime;
    }
    if(moveBackward)
    {
        z -= speed*deltaTime;
    }
    if(moveRight)
    {
        x += speed*deltaTime;
    }
    if(moveLeft)
    {
        x -= speed*deltaTime;
    }
    glm::vec3 newPos = glm::vec3(x, y, z);
    position = newPos;
    updateCamera();
}

void player::updateCamera()
{
    cam->setPosition( position );
}

void player::mouseMotion(int x, int y)
{
    float deltaX = x - lastX;
    float deltaY = y - lastY;
    cout << deltaX << " | " << deltaY << endl;
}
