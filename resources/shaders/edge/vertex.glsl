// input vertex attributes
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 edgeFromPosition;
layout (location = 3) in vec4 edgeToPosition;
layout (location = 4) in vec4 color;


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
out float fBondLength;

vec3 compute_light (out vec3 specular, in vec3 inNormal, in vec3 position);

void main (void)
{
    vec3 center = .5 * (edgeToPosition.xyz + edgeFromPosition.xyz);

    // scale cylinder into z direction
    vec3 vertpos = vPosition;
    vertpos.z *= .5*length(edgeToPosition.xyz - edgeFromPosition.xyz);
    vertpos.xy *= radius;

    vec3 b = edgeToPosition.xyz - edgeFromPosition.xyz;

    vec3 f = vec3(b.x, b.y, 0);
    /*float c3 = b.z;
    float s3 = sqrt(1.f - c3 * c3);
    float c2 = f.x;
    float s2 = -sqrt(1.f - c2 * c2);
    float c1 = 1.;
    float s1 = 0.;*/

    vec3 e3 = vec3(0, 0, 1);
    float cos_gamma = dot (e3, b) / (length(e3) * length(b));
    float sin_gamma = length (cross (e3, b)) / (length(e3) * length(b));


    float gamma = atan(sin_gamma, cos_gamma);
    float theta = atan(f.y, f.x);

    /*float c3 = cos(gamma);
    float s3 = sin(gamma);
    float c2 = cos(theta);
    float s2 = sin(theta);
    float c1 = 1.;
    float s1 = 0.;*/

    float c1 = 1.0;
    float s1 = 0.0;

    float c2 = cos (theta);
    float s2 = sin (theta);

    float c3 = cos (gamma);
    float s3 = sin (gamma);

    /*mat3 rot = mat3(
            c2, -1.*c1 * s2, s2 * s1,
            c3 * s2, c3*c2*c1 - s3*s1, -1.*c1*s3 - c3*c2*c1,
            s3*s2, c3*s1 + c2*c1*s3, c3*c1 - c2*s3*s1
        );*/

    // Y1 X2 Y3
    /*mat3 rot = mat3(c1*c3 - c2*s1*s3, s1*s2, c1*s3+c2*c3*s1,
                    s2*s3, c2, -c3*s2,
                    -c3*s1-c1*c2*s3, c1*s2, c1*c2*c3 - s1*s3);*/

    // Z X Z
    /*mat3 rot = mat3(c1*c3 - c2*s1*s3, -c1*s3 - c2*c3*s1, s1*s2,
                    c3*s1 + c1*c2*s3, c1*c2*c3 - s1*s3, -c1*s2,
                    c2*s3 , c3*s2, c3);*/

    // Y1 Z2 Y3
    mat3 rot = mat3( c1*c2*c3 - s1*s3, -c1 * s2, c3*s1 + c1*c2*s3,
                     c3*s2,            c2,       s2*s3,
                     -c1*s3 - c2*c3*s1, s1*s2, c1*c3 - c2*s1*s3);
    /*mat3 rot = mat3( c2*c3, -s2, c2*s3,
                     s1*s3 + c1*c3*s2, c1*c2, c1*s2*s3 - c3*s1,
                     c3*s1*s2 - c1*s3, c2*s1, c1*c3 + s1*s2*s3);*/

    /*mat3 rot = mat3( c1*c2, s1*s3 - c1*c3*s2, c3*s1 + c1*s2*s3,
                     s2, c2*c3, -c2*s3,
                     -c2*s1, c1*s3 + c3*s1*s2, c1*c3 - s1*s2*s3);*/

    vec4 pos = viewmat * vec4 (center + transpose(rot) * vertpos, 1.0);

    fColor = vec3(.5, .5, .5); // color.rgb

    fPosition = pos.xyz;
    gl_Position = projmat * pos;

    fNormal = mat3 (viewmat) * normalize (rot*normal);

    fBondLength = length(edgeToPosition.xyz - edgeFromPosition.xyz);

}
