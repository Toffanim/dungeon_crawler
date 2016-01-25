#if !defined(CHEST_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define CHEST_H
#include "Actor.h"

class Chest : public Actor
{
public :
    enum Type { GOLD, LIFE, KEY };
    Chest( Model* model, glm::mat4 modelMatrix, Type type, int gift);
    void doCollision(Player* p, float deltaTime );
    void Draw(Shader* s);
private :
    int gift;
    Type giftType;
    bool isOpen;
};

#endif
