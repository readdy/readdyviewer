#version 430 core

// input vertex attributes
layout (location = 0) in vec3 vPosition;
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
out vec3 fColor;

vec3 compute_light (out vec3 specular, in vec3 inNormal, in vec3 position);

void main (void)
{

    vec3 specular;
    vec4 pos;
    if(gl_VertexID < 20) {
        pos = viewmat * vec4 (edgeFromPosition.xyz, 1.0);
    } else {
        pos = viewmat * vec4 (edgeToPosition.xyz, 1.0);
    }
    fColor = normalize(vPosition);

    pos.xyz += vPosition * .1;

    //pos = viewmat * pos;
    //vec3 diffuse = compute_light (specular, mat3 (viewmat) * normalize (normal), pos.xyz/pos.w);

    //fColor = color.rgb;

    vec4 posFrom = viewmat * vec4(edgeFromPosition.xyz, 1.0);
    vec4 posTo = viewmat * vec4(edgeToPosition.xyz, 1.0);


    gl_Position = projmat * pos;

    /*vec4 posFrom = viewmat * vec4(edgeFromPosition.xyz, 1.0);
    vec4 posTo = viewmat * vec4(edgeToPosition.xyz, 1.0);

    vec3 diffuse = compute_light (specular, mat3 (viewmat * mmat) * normalize (normal), pos.xyz/pos.w);

    fColor = diffuse * color.bgr;
    fBackColor = diffuse * backcolor.bgr;

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
	fRadius = particleSize;*/
}
