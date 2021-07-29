#pragma once

#include <GLFW/glfw3.h>
#include <memory>

class Terrain;

struct UpdateInfo {
    GLFWwindow* window;
    double dt;
    std::shared_ptr<Terrain> terrain;
    float daytime;
};

class Updatable {
public:
    Updatable() = default;
    virtual ~Updatable() = default;

    virtual void update(const UpdateInfo&) = 0;
};