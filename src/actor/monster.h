#if !defined(MONSTER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define MONSTER_H
#include "actor.h"

class monster : public actor
{
public :
    monster( Model* model, glm::mat4 modelMatrix, glm::vec3 position,
             int atk, float atkPerSec, int life, float aggroRadius );
    void doCollision( player* p , float deltaTime);
    void checkAggro(player* p, float deltaTime);
    bool isAlive() { return(alive); }
private :
    int life;
    float speed;
    float lastAttack;
    int atk;
    float atkPerSec;
    glm::vec3 position;
    bool alive;
    float aggroRadius;
};
#endif
