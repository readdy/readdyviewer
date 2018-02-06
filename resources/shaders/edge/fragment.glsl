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
	float r = dot (fTexcoord, fTexcoord);

	vec3 normal = vec3 (fTexcoord, sqrt (1 - r));

	vec4 fPos = vec4 (fPosition + fRadius * normal, 1.0);
	vec4 clipPos = projmat * fPos;
	float d = clipPos.z / clipPos.w;
	gl_FragDepth = d*.5 + .5;

	vec3 specular = vec3 (0, 0, 0);
	vec3 light = compute_light (specular, normal, fPos.xyz);

	// fetch texture value and output resulting color
	color = vec4 (light * fColor, 1) + vec4 (specular, 1);
}
