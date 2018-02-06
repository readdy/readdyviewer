#version 430 core

// input vertex attributes
layout (location = 0) in vec2 vPosition;
layout (location = 1) in vec4 edgeFromPosition;
layout (location = 2) in vec4 edgeToPosition;
layout (location = 3) in vec4 color;


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
    float particleSize = 5.f;
	vec4 pos;
	if (gl_VertexID == 3 || gl_VertexID == 2) {
	    pos = viewmat * vec4 (edgeToPosition.xyz, 1.0);
        pos.xy += vPosition * particleSize;
    } else {
        pos = viewmat * vec4 (edgeFromPosition.xyz, 1.0);
        pos.xy += vPosition * particleSize;
    }
	fPosition = pos.xyz;
	fTexcoord = vPosition;
	if (gl_VertexID == 3) {
	    fColor = vec3(0,1,0);
	} else {
	    fColor = vec3(1,0,0);
	}

	// compute and output the vertex position
	// after view transformation and projection
	gl_Position = projmat * pos;
	fRadius = 1;
}
