/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "controller.h"

controller::controller( std::map<SDL_Keycode, std::string> mapping):mapping(mapping)
{

}

event controller::getEventHandler()
{
    return(eventHandler);
}

SDL_Event* controller::getEvents()
{
    return(&events);
}

void controller::processEvents()
{
    while(SDL_PollEvent( &events ))
    {
        switch( events.type ){
            case SDL_KEYDOWN:
            {
                 for (std::map<SDL_Keycode, std::string>::iterator it = mapping.begin();
                     it != mapping.end();
                     ++it)
                {
                    if (events.key.keysym.sym == it->first)
                        eventHandler.fire(it->second);
                }
            }
                printf( "Key press detected\n" );
                break;

            case SDL_KEYUP:
                printf( "Key release detected\n" );
                break;
                
            case SDL_WINDOWEVENT:
            {
                
                    switch (events.window.event) {
                        case SDL_WINDOWEVENT_CLOSE:
                            eventHandler.fire("close");            
                    }
            }
            default:
                break;
        }
    }
}
