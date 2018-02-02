#version 430 core

struct ParticleConfiguration {
    vec4 color;
    float radius;
};

// input vertex attributes
layout (location = 0) in vec2 vPosition;
layout (location = 1) in vec4 particlePosition;
layout (location = 2) in uint highlight;

layout (std430, binding = 0) readonly buffer ParticleConfigurationBuffer {
    ParticleConfiguration particleConfigs[];
};

// projection and view matrix
layout (binding = 0, std140) uniform TransformationBlock {
	mat4 viewmat;
	mat4 projmat;
	mat4 invviewmat;
};

// output to the fragment shader
out vec3 fPosition;
out vec3 fColor;
out vec2 fTexcoord;
out float fRadius;

void main (void)
{
	// pass data to the fragment shader
	float particleSize = particleConfigs[uint(particlePosition.w)].radius;
	vec4 pos = viewmat * vec4 (particlePosition.xyz, 1.0);
	pos.xy += vPosition * particleSize;
	fPosition = pos.xyz;
	fTexcoord = vPosition;
	fColor = particleConfigs[uint(particlePosition.w)].color.xyz;
	if ((highlight&1U) == 1) {
		fColor = vec3 (1, 0, 0);
	} else if ((highlight&2U) ==2) {
		fColor = vec3 (0, 1, 0);
	}

	// compute and output the vertex position
	// after view transformation and projection
	gl_Position = projmat * pos;
	fRadius = particleSize;
}
