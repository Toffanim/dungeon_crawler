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
#include "../mesh/model.h"

struct AABB
{
    glm::vec3 min;
    glm::vec3 max;
};

class actor
{
public :
    actor();
    actor( glm::vec3 position );
    actor( Model* model,glm::mat4 modelMatrix );
    actor( glm::vec3 position, std::string modelPath);
    
    glm::mat4 getModelMatrix() { return(modelMatrix); }
    void setPosition( glm::mat4 newModelMatrix ) { modelMatrix = newModelMatrix; }

private :
    glm::mat4 modelMatrix;
    Model* model;
    AABB aabb;

    void computeAABB();

};
#endif
