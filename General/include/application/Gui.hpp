#pragma once

#include "render/DrawableUnit.hpp"
#include "render/Updatable.hpp"


class Gui: public DrawableUnit, public Updatable {
    
};

class SimpleGui : public Gui {
    virtual void draw(const RenderInfo&);
    virtual void init();
    virtual void update(const UpdateInfo&);
};
