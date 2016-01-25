/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice:  $
   ======================================================================== */
#include "Controller.h"
//The controller is set to control callback from pressed/released key, etc
//Generally has to be bind to a player, but can also be bind to a monster
//to control IA for exemple
Controller::Controller()
{
}

bool Controller::onKeyPress(SDL_Event e)
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

bool Controller::onMiscCallback(unsigned char e)
{
    const auto & func = mappingMisc[e];
    if(func)
    {
        func();
        return(true);
    }
    return(false);
}

void Controller::setMiscCallback(unsigned char code, const std::function<void(void)> &func)
{
    mappingMisc[code] = func;
}

bool Controller::onKeyRelease(SDL_Event e)
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

void Controller::setKeyReleaseCallback( SDL_Keycode code, const std::function<void(void)> &func)
{
    mappingKeyRelease[code] = func;
}


void Controller::setKeyPressCallback( SDL_Keycode code, const std::function<void(void)> &func)
{
    mappingKeyPress[code] = func;
}

SDL_Event* Controller::getEvents()
{
    return(&events);
}

void Controller::setMouseCallback( std::function<void(int, int)> &func )
{
    mouseEvent = func;
}

//Call every frame
void Controller::processEvents()
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
                if ( mouseEvent )
                {
                    mouseEvent( events.motion.xrel, events.motion.yrel );
                }
                break;
        }
    }
}
