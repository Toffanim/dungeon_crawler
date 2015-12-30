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
        speed(2.0f), position( glm::vec3( 0.0f, 0.0f, 0.0f )),
        lastX(0.0f), lastY(0.0f), firstTime(true), life(100), gold(0),
        mapping(std::map<SDL_Keycode, std::string>()), atkPerSec( 0.5f ),
atk( 2 ), lastAttack( 0.0f )
{
//    model = new Model( "Alien_Necromorph/Alien_Necromorph.
#if 1           
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
        c->setKeyPressCallback(SDLK_UP, std::bind(&player::startMoveUp, this));
        c->setKeyReleaseCallback(SDLK_UP, std::bind(&player::stopMoveUp, this));
        c->setKeyPressCallback(SDLK_DOWN, std::bind(&player::startMoveDown, this));
        c->setKeyReleaseCallback(SDLK_DOWN, std::bind(&player::stopMoveDown, this));
        std::function<void(int, int)> f = std::bind(&player::mouseMotion, this, std::placeholders::_1, std::placeholders::_2);
        c->setMouseCallback(f);
        cam = new camera( position );

        aabb.min = glm::vec3();
        aabb.max = glm::vec3();
        aabb.size = glm::vec3();
        aabb.center = glm::vec3();
#endif
}

player::~player()
{
    if(c)
    { delete c; }
    if(cam)
    { delete cam; }
}

bool player::isAttacking( float deltaTime )
{
    lastAttack += deltaTime;
    if ( lastAttack >= atkPerSec)
    {
        lastAttack = 0.0f;
        return true;
    }return false;
}

void player::startMoveUp()
{
    moveUp = true;
}

void player::startMoveDown()
{
    moveDown = true;
}

void player::stopMoveDown()
{
    moveDown=false;
}

void player::stopMoveUp()
{
    moveUp = false;
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
    glm::vec3 u = glm::cross(t,v);
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
    if(moveUp)
    {
        pos += speed*deltaTime*u;
    }
    if(moveDown)
    {
        pos -= speed*deltaTime*u;
    }
    position = pos;
    computeAABB();
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
    cam->updateCameraVectors();    
}


void player::computeAABB()
{
    float halfSize = 0.2f;
    aabb.max.x = position.x + halfSize;
    aabb.max.y = position.y + halfSize;
    aabb.max.z = position.z + halfSize;
    aabb.min.x = position.x - halfSize;
    aabb.min.y = position.y - 0.4f;
    aabb.min.z = position.z - halfSize;

    aabb.size = glm::vec3(halfSize*2);
    aabb.center = position;
}
