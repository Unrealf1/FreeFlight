#pragma once


struct UpdateInfo {

};

class Updatable {
public:
    virtual void update(const UpdateInfo&) = 0;
};