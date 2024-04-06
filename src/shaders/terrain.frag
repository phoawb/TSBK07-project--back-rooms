#version 150

#define MAX_LIGHTS 100 // Use a define to specify the max number of lights

out vec4 outColor;
in vec2 texCoord;
// Ensure these are actually world coordinates
in vec3 exWorldPos; 
in vec3 exWorldNormal;
uniform sampler2D texUnit;

// Light sources
uniform int lightCount; // Actual number of lights in use
uniform vec3 lightSourcesDirPos[MAX_LIGHTS];
uniform vec3 lightSourcesColors[MAX_LIGHTS];
uniform int isDirectional[MAX_LIGHTS];

void main(void) {
    vec3 ambientColor = vec3(0.3); // Ambient color if no lights are affecting the fragment
    vec3 finalColor = ambientColor; // Start with ambient color

    vec3 finalShade = vec3(0.0);
    vec3 ambient = vec3(0.3);
    vec3 diffuse = vec3(0.7);
    vec3 specular = vec3(0.5);
    // ambient, diffuse, specular coefficients
    int k_a = 1, k_d = 1, k_s = 1; 

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

    finalShade = k_a * ambient + k_d * diffuse + k_s * specular;
    
    // Modulate texture color with the calculated lighting
    outColor = texColor * vec4(finalColor, 1.0);
}
