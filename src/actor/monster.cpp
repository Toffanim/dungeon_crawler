/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "monster.h"

monster::monster(Model* model, glm::mat4 modelMatrix, glm::vec3 position, int atk, float atkPerSec, int life, float aggroRadius)
        :actor(model, modelMatrix, "monster"), aggroRadius(aggroRadius), alive(true),
         position(position)
        ,speed(3.0f), lastAttack( 0.0f ), atkPerSec( atkPerSec ), atk(atk), life( life )
{
}

void monster::doCollision( player* p, float deltaTime )
{
    //implement atk/sec
    if (alive )
    {
        lastAttack += deltaTime;
        if ( lastAttack >= atkPerSec )
        {
            p->addLife( -atk );
            lastAttack = 0.0f;
        }
//NOTE(marc) : ne devrais pas être là, si plusieurs monstre attaque le
        //joueur on aura une vitesse d'attaque de fou
        if ( p->isAttacking(deltaTime) )
            life -= p->getAttack();

        if ( life <= 0 )
        {
            alive = false;
            cout << "dead" << endl;
        }
        //donner au joueur son loot
    }
}

void monster::checkAggro(player* p, float deltaTime)
{
    glm::vec3 playerPos = p->getPosition();
    float distance = sqrt( pow(playerPos.x - position.x, 2) + pow(playerPos.z - position.z, 2));
    if ( distance <= aggroRadius && distance >= 0.5)
    {
        glm::vec3 dir = glm::vec3(playerPos.x - position.x, 0.0f, playerPos.z - (position.z));
        dir = glm::normalize(dir);
        glm::vec3 t =  (speed * deltaTime * dir );
        position = position + t;
        glm::mat4 m = glm::mat4();
        m = glm::translate( m, position );
        m = glm::scale( m, glm::vec3(0.1,0.1,0.1));
        setPosition(m);
    }
}
