/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "controller.h"

controller::controller()
{
}

bool controller::onKeyPress(SDL_Event e)
{

    SDL_Keycode code = e.key.keysym.sym;
    const auto & pressFunc = mappingKeyPress[code];
    if(pressFunc)
    {
        pressFunc();
        return(true);
    }
    return(false);
}

bool controller::onMiscCallback(unsigned char e)
{
    const auto & func = mappingMisc[e];
    if(func)
    {
        func();
        return(true);
    }
    return(false);
}

void controller::setMiscCallback(unsigned char code, const std::function<void(void)> &func)
{
    mappingMisc[code] = func;
}

bool controller::onKeyRelease(SDL_Event e)
{

    SDL_Keycode code = e.key.keysym.sym;
    const auto & releaseFunc = mappingKeyRelease[code];
    if(releaseFunc)
    {
        releaseFunc();
        return(true);
    }
    return(false);
}

void controller::setKeyReleaseCallback( SDL_Keycode code, const std::function<void(void)> &func)
{
    mappingKeyRelease[code] = func;
}


void controller::setKeyPressCallback( SDL_Keycode code, const std::function<void(void)> &func)
{
    mappingKeyPress[code] = func;
}

SDL_Event* controller::getEvents()
{
    return(&events);
}

void controller::setMouseCallback( std::function<void(int, int)> &func )
{
    mouseEvent = func;
}

void controller::processEvents()
{
    while(SDL_PollEvent( &events ))
    {
        switch( events.type ){
            case SDL_KEYDOWN:
                onKeyPress(events);
                break;

            case SDL_KEYUP:
                onKeyRelease(events);
                break;
                
            case SDL_WINDOWEVENT:
            {
                onMiscCallback(events.window.event);   
            }
                break;
            case SDL_MOUSEMOTION:
                std::cout << "MOUSE" << std::endl;
                if ( mouseEvent )
                {
                    mouseEvent( events.motion.x, events.motion.y );
                }
                break;
        }
    }
}
