#version 430 core

#define     LIGHT_AMBIENT   0
#define     LIGHT_DIRECTION 1
#define     LIGHT_POINT	    2
#define     LIGHT_HEAD	    3

#define NO_DOUBLE_SIDED_LIGHTING 1
#define USE_DISTANCE 1

// projection and view matrix
layout (binding = 0, std140) uniform TransformationBlock {
	mat4 viewmat;
	mat4 projmat;
	mat4 invviewmat;
};

struct Light {
    vec4 position;
    vec4 direction;
    vec4 color;
    float intensity;
    uint type;
};

layout (std430, binding = 1) readonly buffer LightBuffer {
    Light lights[];
};

vec3 compute_light (out vec3 specularSum, in vec3 inNormal, in vec3 position) {
    vec3 diffuseSum = vec3 (0, 0, 0);
    specularSum = vec3 (0, 0, 0);

    vec3 normal = normalize (inNormal);
    for (int l = 0; l < lights.length(); l++)
    {
        vec3 diffuse = vec3 (0, 0, 0);
        vec3 specular = vec3 (0, 0, 0);
        if (lights[l].type == LIGHT_AMBIENT) {
            diffuse = lights[l].color.xyz;
        } else if (lights[l].type == LIGHT_POINT) {
            vec4 lpos = viewmat * vec4 (lights[l].position.xyz, 1);
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
                specular = 0.1 * attenuation * pow (NdotH, 16) * lights[l].color.xyz;
                diffuse = attenuation * NdotL * lights[l].color.xyz;

            }
        } else if (lights[l].type == LIGHT_HEAD) {
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
                diffuse = NdotL * lights[l].color.xyz;


                specular = pow (NdotH, 16) * lights[l].color.xyz;

                #ifdef USE_DISTANCE
                float attenuation = 25 / dot (vec3 (0, 0.5, 0.0), vec3 (1, dist, dist * dist));
                diffuse *= attenuation;
                specular *= attenuation;
                #endif

            }
        } else if (lights[l].type == LIGHT_DIRECTION) {
            vec3 lightDir = normalize (mat3 (viewmat) * lights[l].direction.xyz);

            vec3 viewDir = normalize (position);
            vec3 halfVec = normalize (lightDir - viewDir);

            #ifdef NO_DOUBLE_SIDED_LIGHTING
            float NdotL = clamp (dot (normal, lightDir), 0, 1);
            float NdotH = clamp (dot (halfVec, normal), 0, 1);
            #else
            float NdotL = abs (dot (normal, lightDir));
            float NdotH = abs (dot (halfVec, normal));
            #endif

            diffuse = NdotL * lights[l].color.xyz;
            specular = pow (NdotH, 16) * lights[l].color.xyz;
        }
        diffuseSum += lights[l].intensity * diffuse;
        specularSum += lights[l].intensity * specular;
    }
    return diffuseSum;
}
