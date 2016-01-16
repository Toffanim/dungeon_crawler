#if !defined(CHEST_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define CHEST_H
#include "actor.h"

class chest : public actor
{
public :
    enum Type { GOLD, LIFE, KEY };
    chest( Model* model, glm::mat4 modelMatrix, Type type, int gift);
    void doCollision(player* p, float deltaTime );
    void Draw(shader* s);
private :
    int gift;
    Type giftType;
    bool isOpen;
};

#endif
