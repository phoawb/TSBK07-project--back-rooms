#version 150

out vec4 outColor;
in vec2 texCoord;
in vec3 exNormal;
uniform sampler2D tex;
uniform sampler2D texBlend;

void main(void)
{
    const vec3 light = vec3(0.58, 0.58, 0.58);
    float shade;
    shade = dot(normalize(exNormal), light);
    // Blend the texture based on height
    // shade = texture(texBlend, texCoord).r;
    outColor = texture(tex, texCoord) * vec4(shade, shade, shade, 1.0);
}
