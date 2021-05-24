#pragma once

#include <GL/glew.h>

struct DrawInfo {
    bool blend = false;
    bool cull = false;
};

class Drawer {
public:
    static void prepareDraw(const DrawInfo& info) {
        if (info.blend) {
            glEnable(GL_BLEND);
        } else {
            glDisable(GL_BLEND);
        }
    }
};