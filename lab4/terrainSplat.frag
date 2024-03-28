#version 150

out vec4 outColor;
in vec2 texCoord;
in vec3 exNormal;
uniform sampler2D map, dirt, grass, conc;

void main(void)
{
    const vec3 light = vec3(0.58, 0.58, 0.58);
    float shade;
    shade = dot(normalize(exNormal), light);
    vec3 m = normalize(texture(map, texCoord).rgb);
    outColor = shade * (texture(dirt, texCoord) * m.r + texture(grass, texCoord*40) * m.g + texture(conc, texCoord*30) * m.b);
    //outColor = texture(map, texCoord) * vec4(shade, shade, shade, 1.0);
}
