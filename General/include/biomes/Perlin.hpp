#pragma once

#include <cstdint>
#include <cmath>

// float linear_mix(float a, float b, float k){
//     return a + (b - a)* k;
// }

// float cos_mix( float a, float b, float x){
//     float f = (float)(( 1.0 - cosf( x * 3.141592)) / 2.0);
//     return (float)(a * (1.0 - f) + b * f);
// }

// float PerlinNoise(float x, float y, float seed_x = 660, float seed_y = 660){
//     float total = 0.0;
//     float pres = (float)0.01; //любое число
//     float ampl = (float)100.2; //любое число
//     float freq = (float)0.06; //любое число
//     int i;

//     for (i = 0; i < 8; i++) {
//         total += final_noise(x * freq + seed_x, y * freq + seed_y) * ampl;
//         ampl *= pres;
//         freq *= 2;
//     }

//     return total;

// };