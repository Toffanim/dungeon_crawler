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
    enum Type { GOLD, LIFE };
    monster( Model* model, glm::mat4 modelMatrix, glm::vec3 position,
             int atk, float atkPerSec, int life, float aggroRadius
             , Type lootType, int lootAmount);
    void doCollision( player* p , float deltaTime);
    void Draw(shader* s);
    void checkAggro(player* p, float deltaTime);
    bool isAlive() { return(alive); }
private :
    Type lootType;
    int lootAmount;
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
