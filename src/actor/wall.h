#if !defined(WALL_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define WALL_H
#include "../player/player.h"
#include "actor.h"

class wall : public actor
{
public :
    wall( Model* model, glm::mat4 modelMatrix );
    void doCollision(player* p, float deltaTime);
    void Draw(shader* s);
private:
    bool isInRange;
};
#endif
