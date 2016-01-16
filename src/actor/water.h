#if !defined(WATER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define WATER_H
#include "actor.h"

class water : public actor
{
public :
    water( Model* model, glm::mat4 modelMatrix);
    void doCollision( player* p, float deltaTime);
    void Draw(shader* s);
};

#endif
