#pragma once

#include <GLFW/glfw3.h>


struct UpdateInfo {
    GLFWwindow* window;
    double dt;
};

class Updatable {
public:
    Updatable() = default;
    virtual ~Updatable() = default;

    virtual void update(const UpdateInfo&) = 0;
};