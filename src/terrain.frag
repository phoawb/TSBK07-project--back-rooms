#version 150

#define MAX_LIGHTS 100 // Use a define to specify the max number of lights

out vec4 outColor;
in vec2 texCoord;
in vec3 exNormal;
uniform sampler2D texUnit;

// light sources
uniform int lightCount; // actual number of lights in use
uniform vec3 lightSourcesDirPos[MAX_LIGHTS];
uniform vec3 lightSourcesColor[MAX_LIGHTS];
//uniform float specularExponent;
uniform bool isDirectional[MAX_LIGHTS];


void main(void)
{
    const vec3 light = vec3(0.58, 0.58, 0.58);
    float shade;
    shade = dot(normalize(exNormal), light);
    outColor = texture(texUnit, texCoord) * vec4(shade, shade, shade, 1.0);
}
