#version 150

out vec4 outColor;
in vec2 texCoord;
uniform sampler2D texUnit;

void main(void) {
    outColor = texture(texUnit, texCoord);
}
