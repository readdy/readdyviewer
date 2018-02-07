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
    vec4 posFrom = viewmat * vec4(edgeFromPosition.xyz, 1.0);
    vec4 posTo = viewmat * vec4(edgeToPosition.xyz, 1.0);

    vec4 midpoint = .5 * edgeFromPosition + .5 * edgeToPosition;
	// pass data to the fragment shader
	float particleSize = 1.;
	vec4 pos = viewmat * vec4 (midpoint.xyz, 1.0);
	if (gl_VertexID == 0 || gl_VertexID == 3) {
	    pos.xyz = posFrom.xyz;
	    pos.x += vPosition[0] * .1;
	    pos.y += vPosition[1] * .1;
	} else {
	    pos.xyz = posTo.xyz;
        pos.x += vPosition[0] * .1;
        pos.y += vPosition[1] * .1;
	}
	fPosition = pos.xyz;
	fTexcoord = vPosition;
	fColor = color.rgb;

	// compute and output the vertex position
	// after view transformation and projection
	gl_Position = projmat * pos;
	fRadius = particleSize;
}
