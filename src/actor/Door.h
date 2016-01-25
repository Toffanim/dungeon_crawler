#if !defined(DOOR_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: $
   $Notice: $
   ======================================================================== */

#define DOOR_H
#include "Actor.h"
class Door : public Actor
{
public :
    Door( Model* model, glm::mat4 modelMatrix);
    void doCollision(Player* p, float deltaTime);
    void Draw(Shader* s);
private :
    bool isOpen;
};
#endif
