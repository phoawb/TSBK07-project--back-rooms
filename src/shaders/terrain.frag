#version 150

#define MAX_LIGHTS 100 // Use a define to specify the max number of lights

out vec4 outColor;
in vec2 texCoord;
in vec3 exWorldPos; // Ensure you're getting the world position from the vertex shader
uniform sampler2D texUnit;

// Light sources
uniform int lightCount; // Actual number of lights in use
uniform vec3 lightSourcesDirPos[MAX_LIGHTS];
uniform vec3 lightSourcesColors[MAX_LIGHTS];
uniform int isDirectional[MAX_LIGHTS];

void main(void) {
    vec3 ambientColor = vec3(0.3); // Ambient color if no lights are affecting the fragment
    vec3 finalColor = ambientColor; // Start with ambient color

    for (int i = 0; i < lightCount; ++i) {
        if (isDirectional[i] == 0) { // Assuming 0 for point lights
            float dist = distance(exWorldPos, lightSourcesDirPos[i]);
            vec3 lightColor = lightSourcesColors[i] * 10.0;
            
            // Simple attenuation based on distance
            float attenuation = 1.0 / (1.0 + dist);
            
            // Modulate light color by attenuation and add to final color
            finalColor += lightColor * attenuation;
        }
    }

    // Fetch the texture color
    vec4 texColor = texture(texUnit, texCoord);
    
    // Modulate texture color with the calculated lighting
    outColor = texColor * vec4(finalColor, 1.0);
}
