#version 430 core

layout (location = 0) out vec4 color;

in vec3 fColor;

layout (early_fragment_tests) in;

void main (void)
{
    color = vec4 (fColor, 1);
}