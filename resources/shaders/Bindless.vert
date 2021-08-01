#version 430

#extension GL_ARB_shader_draw_parameters : enable
#extension GL_ARB_bindless_texture : enable

layout(location = 0) in vec3 vertexPosition;
layout(location = 2) in vec2 texCoord;

struct neightboors_indexes {
    uint left;
    uint far;
    uint right;
    uint near;
};

struct additional_vertex_info {
    float height;
    uvec2 texture;
    uvec2 secondary_texture;
    float secondary_texture_weight;
    neightboors_indexes indexes;
};

layout(std430, binding = 3) buffer additional_data {
    additional_vertex_info additional_vertex_data[];
};

out vec3 colour;
out vec2 TexCoord;

out flat sampler2D ourTexture;
out flat sampler2D secondaryTexture;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 models[100];
uniform uint offsets[100];
uniform mat3 normalToCamera[100];
uniform uint chunk_size;
uniform float step_len;

out float secondary_texture_weight;

out vec3 normalCamSpace; //нормаль в системе координат камеры
out vec4 posCamSpace; //координаты вершины в системе координат камеры

out vec3 debug;


/*
My vector space:
X to the right
Y from the screen
Z up
*/

void main() {
    int height_index = int(vertexPosition.z);

    mat4 model = models[gl_InstanceID];    
    uint offset = offsets[gl_InstanceID];

    uint data_index = offset + height_index;

    float height = additional_vertex_data[data_index].height;
    ourTexture = sampler2D(additional_vertex_data[data_index].texture);
    uint left_index = additional_vertex_data[data_index].indexes.left; // vertex to the left
    uint near_index = additional_vertex_data[data_index].indexes.near; // vertex closer to the screen(in initial coordinates)
    uint right_index = additional_vertex_data[data_index].indexes.right;
    uint far_index = additional_vertex_data[data_index].indexes.far;

    secondaryTexture = sampler2D(additional_vertex_data[data_index].secondary_texture);
    secondary_texture_weight = additional_vertex_data[data_index].secondary_texture_weight;

    vec3 final_pos = vec3(vertexPosition.xy, height);
    gl_Position = projection * view * model * vec4(final_pos, 1.0);

    vec3 grad1 = vec3((2 * step_len), 0.0,  additional_vertex_data[right_index].height - additional_vertex_data[left_index].height) / (step_len * 2.0);
    vec3 grad2 = vec3(0.0, (2 * step_len),  additional_vertex_data[far_index].height - additional_vertex_data[near_index].height) / (step_len * 2.0);
    vec3 norm = normalize(cross(grad1, grad2));
    norm.z = abs(norm.z);
    debug = norm;

    colour = vec3((height + 1.0) / 20.0, height / 20.0, 1.0/(100.0 + height));
    TexCoord = texCoord;
    posCamSpace = view * model * vec4(final_pos, 1.0); //преобразование координат вершины в систему координат камеры
    mat3 normalToCameraMatrix = normalToCamera[gl_InstanceID];
	normalCamSpace = normalize(normalToCameraMatrix * norm); //преобразование нормали в систему координат камеры
}

