#version 330

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 texCoord;

out vec3 colour;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
    colour = (projection * view * model * vec4(norm, 0.0)).xyz * 0.5 + vec3(0.5);
    TexCoord = texCoord;
}