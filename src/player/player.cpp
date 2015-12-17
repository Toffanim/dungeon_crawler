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
        speed(6.0f), position( glm::vec3( 0.0f, 0.0f, 0.0f )),
        lastX(0.0f), lastY(0.0f), firstTime(true)
{
//    model = new Model( "Alien_Necromorph/Alien_Necromorph.obj" );
    
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

    glm::vec3 t = cam->getFront();
    t.y = 0.0;
    t = glm::normalize(t);
    glm::vec3 v = glm::vec3( t.z, t.y, -t.x); 
    if(moveForward)
    {
        pos += speed*deltaTime*t;
    }
    if(moveBackward)
    {
        pos -= speed*deltaTime*t;
    }
    if(moveRight)
    {
        pos += speed*deltaTime*v;
    }
    if(moveLeft)
    {
        pos -= speed*deltaTime*v;
    }
    position = pos;
    updateCamera();
}

void player::updateCamera()
{
    cam->setPosition( position );
}

void player::mouseMotion(int x, int y)
{
    //Not using x and y because SDL is bugged in relative coordinates
    //Create dependency but better for motion use
    int t1;
    int t2;
    SDL_GetRelativeMouseState(&t1,&t2);
    float sensitivity = 0.05;
    t1 *= sensitivity;
    t2 *= -sensitivity;

    cam->addYaw( t1 );
    cam->addPitch( t2 );

/*    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
    pitch = -89.0f; */

    cam->updateCameraVectors();    
}
