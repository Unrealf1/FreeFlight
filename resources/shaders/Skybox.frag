#version 330 core
out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skybox_day;
uniform samplerCube skybox_night;
uniform float night_weight;

void main() {    
    FragColor = mix(texture(skybox_day, texCoords), texture(skybox_night, texCoords), night_weight);
}