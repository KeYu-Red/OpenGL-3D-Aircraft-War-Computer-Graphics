#version 450 core
layout (location=0) in vec3 aPos;
layout (location=1) in vec2 atexCoords;

out VS_OUT{
    vec3 FragPos;
    vec2 TexCoords;
} vs_out;

uniform float scale;
uniform vec3 offset;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(){
    vs_out.FragPos = vec3(model * vec4(scale*aPos, 1.0))+offset;
    vs_out.TexCoords = atexCoords;

    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}
