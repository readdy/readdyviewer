// enable early z culling
layout (early_fragment_tests) in;

// output color
layout (location = 0) out vec4 color;

// texture
layout (binding = 0) uniform sampler2D tex;

// input from vertex shader
in vec2 fTexcoord;
in vec3 fPosition;
in vec3 fNormal;

// projection and view matrix
layout (binding = 0, std140) uniform TransformationBlock {
	mat4 viewmat;
	mat4 projmat;
	mat4 invviewmat;
};

vec3 compute_light (out vec3 specular, in vec3 inNormal, in vec3 position);

void main (void)
{
	// lighting calculations
	// obtain light direction and distance

	vec3 specular = vec3(0,0,0);
	vec3 light = compute_light (specular, fNormal, fPosition.xyz);

	// fetch texture value and output resulting color
	color = vec4 (light, 1) * texture (tex, fTexcoord);
}
