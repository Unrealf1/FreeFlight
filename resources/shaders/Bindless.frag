#version 430

#extension GL_ARB_bindless_texture : enable

in vec3 colour;
in float true_height;
in vec2 TexCoord;

out vec4 fragColor;

flat in sampler2D ourTexture;
flat in sampler2D nearTexture;
flat in float mix_textures;


void main() {
    //fragColor = texture(ourTexture, TexCoord);
    fragColor = texture(ourTexture, TexCoord) * 0.95 + vec4(colour, 1.0) * 0.05;
    if (mix_textures > 0.0) {
        vec4 other_color = texture(nearTexture, TexCoord);
        fragColor = mix(fragColor, other_color, 0.2);
    }
    
    //fragColor = vec4(colour, 1.0);
}