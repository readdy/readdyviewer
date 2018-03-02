// output color
layout (location = 0) out vec4 color;

// input from vertex shader
in vec3 fColor;
in vec3 fNormal;
in vec3 fPosition;
in float fBondLength;

// projection and view matrix
layout (binding = 0, std140) uniform TransformationBlock {
	mat4 viewmat;
	mat4 projmat;
	mat4 invviewmat;
};

vec3 compute_light (out vec3 specular, in vec3 inNormal, in vec3 position);

void main (void)
{
    if(fBondLength >= cutoff)
        discard;

    vec3 specular;
    vec3 diffuse = compute_light (specular, fNormal, fPosition);
    color = vec4 (diffuse * fColor + specular, 1);
}
