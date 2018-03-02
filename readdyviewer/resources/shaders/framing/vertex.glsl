// input vertex attributes
layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec2 vTexcoord;
layout (location = 2) in vec3 vNormal;

// projection and view matrix
layout (binding = 0, std140) uniform TransformationBlock {
	mat4 viewmat;
	mat4 projmat;
	mat4 invviewmat;
};

// output to the fragment shader
out vec2 fTexcoord;
out vec3 fNormal;
out vec3 fPosition;

void main (void)
{
	// pass data to the fragment shader
	fTexcoord = vTexcoord;
	fNormal = mat3(viewmat) * vNormal;
	vec4 pos = viewmat * vec4(vPosition.xyz, 1);
	fPosition = pos.xyz;
	// compute and output the vertex position
	// after view transformation and projection
	gl_Position = projmat * pos;
}
