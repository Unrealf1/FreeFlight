#version 430

layout(location = 0) in vec3 vertexPosition;
layout(std430, binding = 3) buffer terrain_heights {
    float terrain_heights_data[];
};

out vec3 colour;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 models[100];
uniform int offsets[100];
uniform int chunk_size;

out float true_height;



/*
My vector space:
X to the right
Y from the screen
Z up
*/

void main() {
    /*
        (0, 0), (0, 1) ...
        (1, 0), (1, 1) ...
        ...
    */

    float min_position_value = -0.5;
    float max_position_value = 0.5;
    float diff_in_pos_val = max_position_value - min_position_value;
    float x_ratio = (vertexPosition.x - min_position_value) / diff_in_pos_val;
    float y_ratio = (vertexPosition.y - min_position_value) / diff_in_pos_val;
    int height_index = int(vertexPosition.z);

    mat4 model = models[gl_InstanceID];
    int offset = offsets[gl_InstanceID];
    float height = terrain_heights_data[offset + height_index];
    true_height = height;
    vec3 final_pos = vec3(vertexPosition.xy, height);
    gl_Position = projection * view * model * vec4(final_pos, 1.0);
    //colour = vec3(x_ratio, y_ratio, height_index);
    colour = vec3((height + 1.0) / 100.0, 1.0, 0.0);
}