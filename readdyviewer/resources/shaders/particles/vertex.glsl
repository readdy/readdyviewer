struct ParticleConfiguration {
    vec4 color;
    float radius;
};

// input vertex attributes
layout (location = 0) in vec2 vPosition;
layout (location = 1) in vec4 boxOffset;
layout (location = 2) in vec4 particlePosition;

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
	vec4 pos = viewmat * vec4 (particlePosition.xyz + boxOffset.xyz, 1.0);
	pos.xy += vPosition * particleSize;
	fPosition = pos.xyz;
	fTexcoord = vPosition;
	fColor = particleConfigs[uint(particlePosition.w)].color.xyz;

	// compute and output the vertex position
	// after view transformation and projection
	gl_Position = projmat * pos;
	fRadius = particleSize;
}
