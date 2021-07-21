#version 430

#extension GL_ARB_shader_draw_parameters : enable
#extension GL_ARB_bindless_texture : enable

layout(location = 0) in vec3 vertexPosition;
layout(std430, binding = 3) buffer terrain_heights {
    float terrain_heights_data[];
};

layout(std430, binding = 4) buffer textures {
    uvec2 textures_data[];
};

layout(location = 2) in vec2 texCoord;

out vec3 colour;
out vec2 TexCoord;
out flat sampler2D ourTexture;
out flat sampler2D nearTexture;
out flat float mix_textures;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 models[100];
uniform uint offsets[100];
uniform uint chunk_size;

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
    uint offset = offsets[gl_InstanceID];

    uint data_index = offset + height_index;
    float height = terrain_heights_data[data_index];
    ourTexture = sampler2D(textures_data[data_index]);
    uint near_index = 0;
    if (data_index > 0) {
        near_index = data_index - 1;
    }
    ourTexture = sampler2D(textures_data[data_index]);
    nearTexture = sampler2D(textures_data[near_index]);

    mix_textures = 0.0;
    if (textures_data[data_index] != textures_data[near_index]) {
        mix_textures = 1.0;
    }

    true_height = height;
    vec3 final_pos = vec3(vertexPosition.xy, height);
    gl_Position = projection * view * model * vec4(final_pos, 1.0);
    //vec4 world_pos = model * vec4(final_pos, 1.0);
    //colour = vec3(x_ratio, y_ratio, height_index);
    colour = vec3((height + 1.0) / 20.0, height / 20.0, 1.0/(100.0 + height));
    TexCoord = texCoord;
}

