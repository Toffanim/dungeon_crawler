/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "actor.h"

actor::actor( Model* model, glm::mat4 modelMatrix)
        : model(model), modelMatrix(modelMatrix)
{
    computeAABB();
}

void actor::computeAABB()
{
    
    aabb.min = glm::vec3( FLT_MAX, FLT_MAX, FLT_MAX);
    aabb.max = glm::vec3( FLT_MIN, FLT_MIN, FLT_MIN);
    vector<Mesh> m = model->getMeshes();
    for( vector<Mesh>::iterator it = m.begin();
         it != m.end();
         ++it)
    {
        vector<Vertex> v = (*it).vertices;
        for( vector<Vertex>::iterator it = v.begin();
             it != v.end();
             ++it)
        {
            glm::vec4 vertex = modelMatrix * glm::vec4( (*it).Position, 1.0f);
            if ( vertex.x > aabb.max.x ) { aabb.max.x = vertex.x; }
            if ( vertex.y > aabb.max.y ) { aabb.max.y = vertex.y; }
            if ( vertex.z > aabb.max.z ) { aabb.max.z = vertex.z; }
            if ( vertex.x < aabb.min.x ) { aabb.min.x = vertex.x; }
            if ( vertex.y < aabb.min.y ) { aabb.min.y = vertex.y; }
            if ( vertex.z < aabb.min.z ) { aabb.min.z = vertex.z; }
        }
    }    
    //glm::vec3 size = glm::vec3(max.x-min.x, max.y-min.y, max.z-min.z);
    //glm::vec3 center = glm::vec3((min.x+max.x)/2, (min.y+max.y)/2, (min.z+max.z)/2);
    //glm::mat4 transform =  glm::translate(glm::mat4(1), center)* glm::scale(glm::mat4(1), size);

}
