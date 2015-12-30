#if !defined(AABB_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define AABB_H
#include <glm/glm.hpp>
struct AABB
{
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 size;
    glm::vec3 center;
};
#endif
