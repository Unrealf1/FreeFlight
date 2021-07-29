#pragma once

#include "render/Updatable.hpp"
#include "light/Light.hpp"


class SelestialLight: public Updatable {
public:
    SelestialLight(float dawn_intensity, float daylength)
    : _dawn_addition{dawn_intensity, 0.0, -dawn_intensity}, _daylength(daylength) {
        
    }

    void update(const UpdateInfo& info) override {
        float max_dist = glm::pi<float>();

        float dist = std::min(_theta - _dawn_primetime, max_dist - _theta);
        if (dist < 0.0f) {
            dist = max_dist + dist;
        }
        float dawn_k = 1.0f - (dist / max_dist) * 2.0f;
        _light->diffuse = day ? _diffuse_daylight : _diffuse_moonlight;
        _light->diffuse += dawn_k * _dawn_addition;

        _light->direction = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta));
        //spdlog::info("dawn k is {}, dist is {}", dawn_k, dist);
        _theta += glm::pi<float>() * 2.0f * static_cast<float>(info.dt) / _daylength;
        if (_theta > max_dist) {
            _theta -= max_dist;
            day = !day;
        }
    }

    // 0.0 = dawn, 1.0 = sunset, 2.0 = dawn
    // float between 0.0 and 2.0
    float getDayTime() {
        float ratio = _theta / glm::pi<float>();
        return day ? ratio : ratio + 1.0f;
    }

    std::shared_ptr<const DirectionalLight> getLightInfo() {
        return _light;
    }

    // these are public for debug purposes and to allow
    // sun manipulation via gui sliders. Generally shouldn't 
    // be modified by outsiders
    float _phi = 0.0;
    float _theta = glm::pi<float>() * 0.25f;
private:
    // 24 hours will go in this many seconds
    const float _daylength;
    bool day = true;

    const glm::vec3 _diffuse_daylight{0.8, 0.8, 0.8};
    const glm::vec3 _diffuse_moonlight{0.3, 0.4, 0.6};

    const float _dawn_primetime = 0.0f;
    const glm::vec3 _dawn_addition;

    std::shared_ptr<DirectionalLight> _light = std::make_shared<DirectionalLight>();
};