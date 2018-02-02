#version 430 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 color;

// projection and view matrix
layout (binding = 0, std140) uniform TransformationBlock {
	mat4 viewmat;
	mat4 projmat;
	mat4 invviewmat;
};

out vec3 fColor;

void main (void)
{
    // projmat * viewmat *
    gl_Position = projmat * viewmat * position;
    //gl_Position = vec4(0, 0, 0, 0);
    fColor = vec3(1,0,0); // color.bgr
}
