#version 460 core

in vec2 TexCoord;
out vec4 FragColor;

uniform usampler2D uTexture;
uniform vec3 aliveCellColor;
uniform vec3 deadCellColor;

void main()
{
    float value = float(texture(uTexture, TexCoord).r);

    vec3 cellColor = mix(deadCellColor, aliveCellColor, value);

    FragColor = vec4(cellColor, 1);
}