#version 330

in vec3 colour;
in vec2 TexCoord;

out vec4 fragColor;

uniform sampler2D ourTexture;

void main() {
    fragColor = texture(ourTexture, TexCoord);
    if (fragColor.a == 0) {
        discard;
    }
}