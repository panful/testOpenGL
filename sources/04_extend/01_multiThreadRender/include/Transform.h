#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp> // glm::vec3
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/scalar_constants.hpp> // glm::pi
class Transformation
{
public:
    Transformation();

    ~Transformation() = default;

    void rotate(int deltaX, int deltaY);

    void translate(int deltaX, int deltaY);

    void zoomAction(int wheel);

    void reset();

    glm::mat4 getModelViewMatrix() const ;

    static void tick();



private:
    glm::quat gquat;
    glm::vec3 translation{0,0,0};
    glm::vec3 zoom{1,1,1};
    glm::vec3 cameraLoc{};
    glm::vec3 cameraTarget{0,0,-1};
    glm::vec3 cameraUp{0,1,0};

public:
    static float zoomCoef;
    static float rotateSpd;
    static float transSpd;
};