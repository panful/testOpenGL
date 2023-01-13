#include "Transform.h"
#include <iostream>

float Transformation::zoomCoef = 1.05;
float Transformation::rotateSpd = 0.003;
float Transformation::transSpd = 1;

Transformation::Transformation()
{
    reset();
}

void Transformation::rotate(int deltaX, int deltaY)
{
    if (deltaX != 0)
    {
        gquat = glm::angleAxis(-rotateSpd * deltaX, glm::vec3(0.f, 1.f, 0.f)) * gquat;
    }
    if (deltaY != 0)
    {
        gquat = glm::angleAxis(rotateSpd * deltaY, glm::vec3(1.f, 0.f, 0.f)) * gquat;
    }
}

void Transformation::translate(int deltaX, int deltaY)
{
    translation += glm::vec3(deltaX * transSpd, deltaY * transSpd, 0);
    
}

void Transformation::zoomAction(int wheel)
{
    if(wheel>0)
        zoom = zoom * (zoomCoef * wheel);
    else
        zoom = zoom / (zoomCoef * -wheel);
    
}

void Transformation::reset()
{
    gquat = glm::angleAxis(0.f, glm::vec3(0.f, 1.f, 0.f));
    translation = { 0,0,0 };
    zoom = { 1,1,1 };
}

glm::mat4 Transformation::getModelViewMatrix() const
{
    glm::mat4 tmp{ 1.0f };
    glm::mat4 rotMat= glm::mat4_cast(gquat);
    glm::mat4 transMat = glm::translate(tmp, translation);
    glm::mat4 zoomMat = glm::scale(tmp, zoom);
    glm::mat4 viewMat = glm::lookAt(cameraLoc, cameraTarget, cameraUp);

    return viewMat*zoomMat*transMat * rotMat;
}

