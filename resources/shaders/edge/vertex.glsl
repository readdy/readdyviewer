#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

layout (std140, binding = 0) uniform CameraBuffer {
    mat4 projmat;
    mat4 viewmat;
    mat4 viewmatinv;
};

layout (std140, binding = 1) uniform GeometryBuffer {
    mat4 mmat;
};

out vec3 fColor;

void main (void)
{
    gl_Position = projmat * viewmat * mmat * vec4 (position, 1);
    fColor = color.bgr;
}