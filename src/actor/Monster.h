#if !defined(MONSTER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice: $
   ======================================================================== */

#define MONSTER_H
#include "Actor.h"

class Monster : public Actor
{
    
public :
    enum Type { GOLD, LIFE };
    Monster( Model* model, glm::mat4 modelMatrix, glm::vec3 position,
             int atk, float atkPerSec, int life, float aggroRadius
             , Type lootType, int lootAmount);
    void doCollision( Player* p , float deltaTime);
    void Draw(Shader* s);
    void checkAggro(Player* p, float deltaTime);
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
