struct ParticleConfiguration {
    vec4 color;
    float radius;
};

// input vertex attributes
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 posType;

// projection and view matrix
layout (binding = 0, std140) uniform TransformationBlock {
	mat4 viewmat;
	mat4 projmat;
	mat4 invviewmat;
};

// output to the fragment shader
out vec3 fColor;
out vec3 fNormal;
out vec3 fPosition;

void main (void)
{
    float particleSize = particleConfigs[uint(posType.w)].radius;
    vec4 pos = viewmat * vec4 (posType.xyz, 1.0);
    pos.xyz += vPosition * particleSize;

    fPosition = pos.xyz;
    fColor = particleConfigs[uint(particlePosition.w)].color.xyz;
    fNormal = mat3(viewmat) * normal;

    gl_Position = projmat * pos;
}
