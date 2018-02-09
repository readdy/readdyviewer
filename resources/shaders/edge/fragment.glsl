#version 430 core

// output color
layout (location = 0) out vec4 color;

// input from vertex shader
in vec3 fPosition;
in vec3 fColor;
in vec2 fTexcoord;
in float fRadius;

// projection and view matrix
layout (binding = 0, std140) uniform TransformationBlock {
	mat4 viewmat;
	mat4 projmat;
	mat4 invviewmat;
};

vec3 compute_light (out vec3 specular, in vec3 inNormal, in vec3 position);

void main (void)
{
    color = vec4 (fColor, 1);
}
