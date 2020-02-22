#version 450 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;
uniform vec3 offSet;

void main()
{
    gl_Position = lightSpaceMatrix * (model * vec4(aPos, 1.0)+vec4(offSet,1.0f));
}