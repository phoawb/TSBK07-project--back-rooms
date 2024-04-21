#version 150

in vec2 outTexCoord;
uniform sampler2D texUnit;
out vec4 out_Color;

void main(void)
{
     int size = 5;
    vec2 tex_offset = 1.0 / textureSize(texUnit, 0); // gets size of single texel
    vec3 result = vec3(0.0);
    
    result += texture(texUnit, outTexCoord - vec2(0.0, 2.0 * tex_offset.y)).rgb;
    result += 4.0 * texture(texUnit, outTexCoord - vec2(0.0, tex_offset.y)).rgb;
    result += 6.0 * texture(texUnit, outTexCoord).rgb;
    result += 4.0 * texture(texUnit, outTexCoord + vec2(0.0, tex_offset.y)).rgb;
    result += texture(texUnit, outTexCoord + vec2(0.0, 2.0 * tex_offset.y)).rgb;

    out_Color = vec4(result/16.0, 1.0); // normalize by the sum of the weights
}