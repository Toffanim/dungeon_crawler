/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "door.h"

door::door( Model* model, glm::mat4 modelMatrix)
        : actor(model,modelMatrix), isOpen(false)
{

}

void door::doCollision(player* p, float deltaTime)
{
    if(!isOpen)
    {
        cout << "door collision" << endl;
        //rendre texture du model transparent
        isOpen = true;
    }
}
