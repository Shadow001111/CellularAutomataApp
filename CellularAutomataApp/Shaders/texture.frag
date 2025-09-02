#version 460 core
in vec2 TexCoord;
out vec4 FragColor;
uniform usampler2D uTexture;
void main() {
    uint value = texture(uTexture, TexCoord).r;
    FragColor = vec4(value, value, value, 1);
}