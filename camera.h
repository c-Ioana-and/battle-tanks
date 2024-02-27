#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"


namespace implemented
{
    class Camera1
    {
     public:
        Camera1()
        {
            position    = glm::vec3(0, 2, 5);
            forward     = glm::vec3(0, 0, -1);
            up          = glm::vec3(0, 1, 0);
            right       = glm::vec3(1, 0, 0);
            distanceToTarget = 4;
        }

        Camera1(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
        {
            Set(position, center, up);
        }

        ~Camera1()
        { }

        // Update camera
        void Set(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
        {
            this->position = position;
            forward     = glm::normalize(center - position);
            right       = glm::cross(forward, up);
            this->up    = glm::cross(right, forward);
        }

        void TranslateForward(float distance)
        {
            // Translate the camera using the `forward` vector.
            glm::vec3 forward = glm::normalize(glm::vec3(this->forward.x, 0, this->forward.z));
            position += forward * distance;
        }

        void RotateThirdPerson_OX(float angle, float ofX, float ofZ)
        {
            // Rotate the camera in third-person mode around
            // the object's OX axis

            // calculate the forward, right and up vectors for the tank
            glm::vec3 tankForward = glm::normalize(glm::vec3(sin(ofX), 0, cos(ofX)));
            glm::vec3 tankRight = glm::normalize(glm::cross(tankForward, glm::vec3(0, 1, 0)));
            glm::vec3 tankUp = glm::normalize(glm::cross(tankRight, tankForward));

            // calculate the new forward angle for the camera
            forward = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, tankRight) * glm::vec4(forward, 1)));
            up = tankUp;
            right = tankRight;
        }

        void RotateThirdPerson_OY(float angle, float ofX, float ofZ)
        {
            // Rotate the camera in third-person mode around
            // the OY axis.

            // calculate the forward, right and up vectors for the tank
            glm::vec3 tankForward = glm::normalize(glm::vec3(sin(ofX), 0, cos(ofX)));
            glm::vec3 tankRight = glm::normalize(glm::cross(tankForward, glm::vec3(0, 1, 0)));
            glm::vec3 tankUp = glm::normalize(glm::cross(tankRight, tankForward));

            // calculate the new forward angle for the camera
            forward = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, tankUp) * glm::vec4(forward, 1)));
            up = tankUp;
            right = tankRight;
        }

        glm::mat4 GetViewMatrix()
        {
            // Returns the view matrix
            return glm::lookAt(position, position + forward, up);
        }

        glm::vec3 GetTargetPosition()
        {
            return position + forward * distanceToTarget;
        }

     public:
        float distanceToTarget;
        glm::vec3 position;
        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up;
    };
}   // namespace implemented
