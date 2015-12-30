#if !defined(ACTOR_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define ACTOR_H
#include <string>
#include <algorithm>
#include <stdlib.h>
#include "../mesh/aabb.h"
#include "../mesh/model.h"
#include "../player/player.h"

class player;

class actor
{
public :
    actor();
    actor( glm::vec3 position );
    actor( Model* model,glm::mat4 modelMatrix );
    actor( glm::vec3 position, std::string modelPath);
    
    glm::mat4 getModelMatrix() { return(modelMatrix); }
    Model* getModel(){return(model);}
    void setPosition( glm::mat4 newModelMatrix ) { modelMatrix = newModelMatrix; computeAABB();}
    AABB& getAABB(){return(aabb);}

virtual void doCollision( player* p, float deltaTime = 0.0f ) = 0;
private :
    glm::mat4 modelMatrix;
    Model* model;
    AABB aabb;

    void computeAABB();

};
#endif
