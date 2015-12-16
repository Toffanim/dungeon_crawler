#if !defined(CONTROLLER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define CONTROLLER_H
#include <SDL2/SDL.h>
#include "event.h"
#include <vector>
#include <functional>

class context
{
public :
    context(): keyPressEvents(SDL_
}

class controller
{
public:
    controller(std::map<SDL_Keycode, std::string> mapping);
    SDL_Event* getEvents();
    void processEvents();
    event getEventHandler();
private:
    SDL_Event events;
    event eventHandler;
    std::map<SDL_Keycode, std::string> mapping;
};
#endif
