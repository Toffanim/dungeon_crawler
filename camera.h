#if !defined(CAMERA_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define CAMERA_H

#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class camera
{
public :
    camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           GLfloat yaw = 90.0f, GLfloat pitch = 0.0f);

    glm::mat4 getViewMatrix();
    GLfloat getZoom() { return(zoom); }
           
private:
    glm::vec3 position;
    glm::vec3 up;
    GLfloat yaw;
    GLfloat pitch;
    GLfloat zoom;
    glm::vec3 front;
    glm::vec3 right;

    void updateCameraVectors();
};

#endif