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

class controller
{
public:
    controller();
    SDL_Event* getEvents();
    void processEvents();
    bool onKeyPress(SDL_Event e);
    bool onKeyRelease(SDL_Event e);
    bool onMiscCallback(unsigned char e);
    void setKeyReleaseCallback(SDL_Keycode code, const std::function<void(void)> &func);
    void setKeyPressCallback(SDL_Keycode code, const std::function<void(void)> &func);
    void setMiscCallback(unsigned char code, const std::function<void(void)> &func);
    void setMouseCallback(std::function<void(int, int)> &func);
    event getEventHandler();
private:
    SDL_Event events;
    std::map<SDL_Keycode, std::function<void(void)>> mappingKeyRelease;
    std::map<SDL_Keycode, std::function<void(void)>> mappingKeyPress;
    std::map<unsigned char, std::function<void(void)>> mappingMisc;
    std::function<void(int, int)> mouseEvent;
};
#endif
