#if !defined(WATER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: $
   $Notice:  $
   ======================================================================== */

#define WATER_H
#include "Actor.h"

class Water : public Actor
{
public :
    Water( Model* model, glm::mat4 modelMatrix);
    void doCollision( Player* p, float deltaTime);
    void Draw(Shader* s);
};

#endif
