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
        moveForward(false), moveRight(false), moveLeft(false), moveBackward(false)
{
    model = new Model( "Alien_Necromorph/Alien_Necromorph.obj" );
    
    mapping.insert( std::pair<SDL_Keycode,std::string>(forwardKey, "moveForward"));
        mapping.insert( std::pair<SDL_Keycode,std::string>(backwardKey, "moveBackward"));
        mapping.insert( std::pair<SDL_Keycode,std::string>(rightKey, "moveRight"));
        mapping.insert( std::pair<SDL_Keycode,std::string>(leftKey, "moveLeft"));

        c = new controller(mapping);
        c->getEventHandler().Register( "moveForward", [=] { startMoveForward(); });
        c->getEventHandler().Register( "moveBackward", [=] { startMoveBackward(); });
        c->getEventHandler().Register( "moveRight", [=] { startMoveRight();});
        c->getEventHandler().Register( "moveLeft", [=]{ startMoveLeft();});
}

void player::startMoveForward()
{
    cout << "moveforward" << endl;
    moveForward = !moveForward;
}

void player::startMoveBackward()
{
    moveBackward = !moveBackward;
}

void player::startMoveRight()
{
    moveRight = !moveRight;
}

void player::startMoveLeft()
{
    moveLeft = !moveLeft;
}
