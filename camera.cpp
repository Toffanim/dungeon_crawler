/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "camera.h"

camera::camera(glm::vec3 position, glm::vec3 up, GLfloat yaw , GLfloat pitch ) : position(position), up(up), yaw(yaw), pitch(pitch)
{
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    zoom = 45.0f;
    updateCameraVectors();
}

glm::mat4 camera::getViewMatrix()
{
    return glm::lookAt(position, position + front, up);
}

void camera::updateCameraVectors()
{
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    this->front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(this->front, up));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    up    = glm::normalize(glm::cross(right, this->front));
}
