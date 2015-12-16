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
    void setPosition( glm::vec3 newPos ) { position = newPos; }
    glm::vec3 getPosition(){return(position);}
    void addYaw( float value ) { yaw += value; }
    void addPitch( float value ) { pitch += value; }
    void updateCameraVectors();
    glm::vec3 getFront(){return(front);}
private:
    glm::vec3 position;
    glm::vec3 up;
    GLfloat yaw;
    GLfloat pitch;
    GLfloat zoom;
    glm::vec3 front;
    glm::vec3 right;

    
};

#endif
