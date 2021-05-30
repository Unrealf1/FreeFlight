#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>


class ObjectMover {
public:
    ObjectMover() = default;
    virtual ~ObjectMover() = default;

    virtual void update(GLFWwindow* window, double dt) = 0;

    virtual glm::mat4 getModelMat() = 0;
};

class FreeObjectMover : public ObjectMover {
public:
    FreeObjectMover(float speed = 10.0f): _speed(speed) {}

    void setPosition(glm::vec3 pos) {
        _pos = pos;
    }

    void setSpeed(float speed) {
        _speed = speed;
    }

    glm::vec3 getPosition() {
        return _pos;
    }

    glm::mat4 getModelMat() override {
        return glm::toMat4(-_rot) * glm::translate(-_pos);
    }

    void update(GLFWwindow* window, double dt) override {
        float actual_speed = _speed;
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            actual_speed *= 2;
        }

        glm::vec3 forwDir = glm::vec3(0.0f, 0.0f, -1.0f) * _rot;

        glm::vec3 rightDir = glm::vec3(1.0f, 0.0f, 0.0f) * _rot;

        glm::vec3 upDir = glm::vec3(0.0f, 1.0f, 0.0f) * _rot;
        
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            _pos += forwDir * actual_speed * static_cast<float>(dt);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            _pos -= forwDir * actual_speed * static_cast<float>(dt);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            _pos -= rightDir * actual_speed * static_cast<float>(dt);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            _pos += rightDir * actual_speed * static_cast<float>(dt);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || 
            glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
            _pos -= upDir * actual_speed * static_cast<float>(dt);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            _pos += upDir * actual_speed * static_cast<float>(dt);
        }
    }

private:
    float _speed;
    glm::vec3 _pos;
    glm::quat _rot;

    //Положение курсора мыши на предыдущем кадре
    double _oldXPos = 0.0;
    double _oldYPos = 0.0;
};
