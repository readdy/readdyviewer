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
    float height = .1;
	vec4 pos;
	if (gl_VertexID == 0 || gl_VertexID == 3) {
	    pos = viewmat * vec4 (edgeToPosition.xyz, 1.0);
	    if (gl_VertexID == 0) {
	        pos.y += vPosition[1] * height;
	    } else {
            pos.y -= vPosition[1] * height;
	    }
        //pos.xy += vPosition * particleSize;
    } else {
        pos = viewmat * vec4 (edgeFromPosition.xyz, 1.0);
        if (gl_VertexID == 1) {
            pos.y += vPosition[1] * height;
        } else {
            pos.y -= vPosition[1] * height;
        }
    }
	fPosition = pos.xyz;
	fTexcoord = vPosition;
	//if (gl_VertexID == 3) {
	//    fColor = vec3(0,1,0);
	//} else {
	//    fColor = vec3(1,0,0);
	//}
	fColor = color.xyz;

	// compute and output the vertex position
	// after view transformation and projection
	gl_Position = projmat * pos;
	fRadius = 1;
}
