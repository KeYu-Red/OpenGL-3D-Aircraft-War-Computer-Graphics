#version 450 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in VS_OUT{
    vec3 FragPos;
    vec2 TexCoords;
} fs_in;

uniform sampler2D sprite;
uniform vec4 color;

void main()
{
    FragColor = texture(sprite, fs_in.TexCoords) * color;
    //vec4(ParticalColor.r,ParticalColor.g,ParticalColor.b,0.0f));
    //color = vec4(ParticalColor.r,ParticalColor.g,ParticalColor.b,0.0f);
    float brightness = dot(color.rgb,vec3(0.2126,0.7152,0.0722));
    if(brightness>1.0)
        BrightColor = vec4(color.rgb,color.a);
    else
        BrightColor = vec4(0.0,0.0,0.0,1.0);
}