#if !defined(DOOR_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define DOOR_H
#include "actor.h"
class door : public actor
{
public :
    door( Model* model, glm::mat4 modelMatrix);
    void doCollision(player* p, float deltaTime);
private :
    bool isOpen;
};
#endif
