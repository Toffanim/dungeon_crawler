/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "water.h"

water::water( Model* model, glm::mat4 modelMatrix ) :
        actor(model, modelMatrix, "water")
{

}

void water::doCollision(player* p, float deltatTime)
{

}
