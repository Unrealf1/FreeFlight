#pragma once

#include <cstdint>


class Generator {
public:
    float randomFloat(float from, float to) {
        ++_counter;

        return from + (to - from) * (static_cast<float>(randomInt(0, 191919)) / 191919.0f);
    }

    int32_t randomInt(int32_t from, int32_t to) {
        return (rand() + _counter) % (to - from) + from;
    }

private:
    uint64_t _counter;

};
