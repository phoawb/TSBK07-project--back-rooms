#version 150

// model coordinates
in  vec3 inPosition;
in  vec3 inNormal;
in vec2 inTexCoord;

out vec2 texCoord;
out vec3 exWorldNormal;

// world coordinates
out vec3 exWorldPos;


uniform mat4 projMatrix;
uniform mat4 mdlMatrix;
uniform mat4 model2World;

void main(void)
{
	exWorldPos = vec3(model2World * vec4(inPosition, 1.0)); // world position to be used in fragment shader
	mat3 normalMatrix = transpose(inverse(mat3(model2World))); // see book page 80, or http://www.lighthouse3d.com/tutorials/glsl-tutorial/the-normal-matrix/
	exWorldNormal = normalize(mat3(normalMatrix) * inNormal);
	texCoord = inTexCoord;
	gl_Position = projMatrix * mdlMatrix * vec4(inPosition, 1.0);
}
