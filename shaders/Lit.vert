#version 330

struct LightInfo {
    vec3 dir;
    vec3 La;
    vec3 Ld;
    vec3 Ls;
    float a0;
    float a1;
    float a2;
};

uniform LightInfo light;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 texCoord;

out vec2 TexCoord;
out vec4 posCamSpace;
out vec3 normalCamSpace; //нормаль в системе координат камеры

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalToCameraMatrix;


void main() {
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
    TexCoord = texCoord;

	posCamSpace = view * model * vec4(vertexPosition, 1.0); //преобразование координат вершины в систему координат камеры

	normalCamSpace = normalize(normalToCameraMatrix * norm); //преобразование нормали в систему координат камеры
    //dirCamSpace = vec3(view * vec4(light.dir, 0.0));
}