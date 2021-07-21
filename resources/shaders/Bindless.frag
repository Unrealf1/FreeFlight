#version 430

#extension GL_ARB_bindless_texture : enable

in vec3 colour;
in float true_height;
in vec2 TexCoord;

out vec4 fragColor;

flat in sampler2D ourTexture;


void main() {
    //fragColor = texture(ourTexture, TexCoord);
    fragColor = texture(ourTexture, TexCoord) * 0.95 + vec4(colour, 1.0) * 0.05;
    //fragColor = vec4(colour, 1.0);
}