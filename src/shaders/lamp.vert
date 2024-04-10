#version 150

// model coordinates
in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;

out vec2 texCoord;

uniform mat4 projMatrix;
uniform mat4 mdlMatrix;

void main(void)
{
  vec3 NO_OP = inNormal; // this is just to suppress the warning
	texCoord = inTexCoord;
	gl_Position = projMatrix * mdlMatrix * vec4(inPosition, 1.0);
}
