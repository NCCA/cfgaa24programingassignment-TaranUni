#version 410 core

layout (location=0) in vec3 inPos;
layout (location=1) in vec3 inColour;

uniform mat4 MVP;

void main()
{
    gl_Position=MVP*vec4(inPos,1);
    objectColour=inColour;
}