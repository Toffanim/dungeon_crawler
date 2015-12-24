/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "chest.h"

chest::chest(Model* model, glm::mat4 modelMatrix, Type type, int gift )
        : actor( model, modelMatrix ), type(type), gift(gift)
{

}
