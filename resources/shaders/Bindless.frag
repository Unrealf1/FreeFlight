#version 430

#extension GL_ARB_bindless_texture : enable

in vec3 colour;
in float true_height;
in vec2 TexCoord;
in float proximity_flag;

out vec4 fragColor;

flat in sampler2D ourTexture;
flat in sampler2D nearTexture;
flat in sampler2D leftTexture;
flat in float mix_textures;
flat in float initial_flag;


void main() {
    vec4 texColor = texture(ourTexture, TexCoord);
    vec4 others = mix(texture(nearTexture, TexCoord), texture(leftTexture, TexCoord), 0.5);

    float tex_coef = proximity_flag;
    if (initial_flag < 1.0) { // means == 0.0, I just don't like comparing floats on equality
        tex_coef = 1.0 - tex_coef;
    }
    texColor = mix(texColor, others, tex_coef);
    fragColor = texColor * 0.95 + vec4(colour, 1.0) * 0.05;
    if (mix_textures > 0.0) {
        // vec4 other_color = texture(nearTexture, TexCoord);
        // fragColor = mix(fragColor, other_color, 0.2);
    }
    
    //fragColor = vec4(colour, 1.0);
}