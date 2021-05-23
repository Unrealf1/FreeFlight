#pragma once

#include "render/RenderInfo.hpp"
#include "render/RenderDefs.hpp"

class DrawableUnit {
public:
    DrawableUnit() = default;
    virtual ~DrawableUnit() = default;

    virtual void draw(const RenderInfo&) = 0;
    virtual void init() = 0;
protected:
    GraphicObject _graphics;
};
