#if !defined(WALL_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice:  $
   ======================================================================== */

#define WALL_H
#include "Actor.h"

class Wall : public Actor
{
public :
    Wall( Model* model, glm::mat4 modelMatrix );
    void doCollision(Player* p, float deltaTime);
    void Draw(Shader* s);
private:
    bool isInRange;
};
#endif
