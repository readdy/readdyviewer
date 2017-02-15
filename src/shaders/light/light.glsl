#version 430 core

#define     LIGHT_AMBIENT   0
#define     LIGHT_DIRECTION 1
#define     LIGHT_POINT	    2
#define     LIGHT_HEAD	    3

#define USE_DISTANCE
#define NO_DOUBLE_SIDED_LIGHTING

// projection and view matrix
layout (binding = 0, std140) uniform TransformationBlock {
	mat4 viewmat;
	mat4 projmat;
	mat4 invviewmat;
};

layout (std140, binding = 1) uniform LightParameters {
    vec4 lightposition;
    vec4 lightdirection;
    vec4 lightcolor;
    float lightintensity;
    uint lighttype;
};

vec3 compute_light (out vec3 specularSum, in vec3 inNormal, in vec3 position)
{
    vec3 diffuseSum = vec3 (0, 0, 0);
    specularSum = vec3 (0, 0, 0);

    vec3 normal = normalize (inNormal);
    vec3 diffuse = vec3 (0, 0, 0);
    vec3 specular = vec3 (0, 0, 0);
    if (lighttype == LIGHT_AMBIENT) {
        diffuse = lightcolor.xyz;
    } else if (lighttype == LIGHT_POINT) {
        vec4 lpos = viewmat * vec4 (lightposition.xyz, 1);
        lpos.xyz /= lpos.w;
        vec3 lightDir = lpos.xyz - position;
        float dist = length (lightDir);

        if (dist > 0) {
            lightDir /= dist;
            vec3 viewDir = normalize (position);
            vec3 halfVec = normalize (lightDir - viewDir);

            #ifdef NO_DOUBLE_SIDED_LIGHTING
            float NdotL = clamp (dot (normal, lightDir), 0, 1);
            float NdotH = clamp (dot (halfVec, normal), 0, 1);
            #else
            float NdotL = abs (dot (normal, lightDir));
            float NdotH = clamp (abs (dot (halfVec, normal)), 0, 1);
            #endif

            float attenuation = 100 / (dist);
            specular = 0.1 * attenuation * pow (NdotH, 16) * lightcolor.xyz;
            diffuse = attenuation * NdotL * lightcolor.xyz;

        }
    } else if (lighttype == LIGHT_HEAD) {
        vec3 lightDir = position;
        float dist = length (lightDir);
        if (dist > 0) {
            lightDir /= dist;

            vec3 viewDir = normalize (position);
            vec3 halfVec = normalize (lightDir + viewDir);

            #ifdef NO_DOUBLE_SIDED_LIGHTING
            float NdotL = clamp (-dot (normal, lightDir), 0, 1);
            float NdotH = clamp (-dot (halfVec, normal), 0, 1);
            #else
            float NdotL = abs (dot (normal, lightDir));
            float NdotH = abs (dot (halfVec, normal));
            #endif
            diffuse = NdotL * lightcolor.xyz;


            specular = pow (NdotH, 16) * lightcolor.xyz;

            #ifdef USE_DISTANCE
            float attenuation = 10 / dot (vec3 (0, 0.5, 0.0), vec3 (1, dist, dist * dist));
            diffuse *= attenuation;
            specular *= attenuation;
            #endif

        }
    } else if (lighttype == LIGHT_DIRECTION) {
        vec3 lightDir = normalize (mat3 (viewmat) * lightdirection.xyz);

        vec3 viewDir = normalize (position);
        vec3 halfVec = normalize (lightDir - viewDir);

        #ifdef NO_DOUBLE_SIDED_LIGHTING
        float NdotL = clamp (dot (normal, lightDir), 0, 1);
        float NdotH = clamp (dot (halfVec, normal), 0, 1);
        #else
        float NdotL = abs (dot (normal, lightDir));
        float NdotH = abs (dot (halfVec, normal));
        #endif

        diffuse = NdotL * lightcolor.xyz;
        specular = pow (NdotH, 16) * lightcolor.xyz;
    }
    diffuseSum += lightintensity * diffuse;
    specularSum += lightintensity * specular;
    return diffuseSum;
}
