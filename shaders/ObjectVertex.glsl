#version 410 core

layout (location=0) in vec4 inPos;
layout (location=1) in vec4 inColour;
out vec4 objectColour;
uniform mat4 MVP;

void main()
{
    gl_Position=MVP*vec4(inPos.xyz,1);
    gl_PointSize=inPos.w;
    inColour = vec4(1.0f,1.0f,1.0f,1.0f);
    objectColour=inColour;
//    objectColour=vec3(1,1,1);
//
//    objectColour = inColour;
//
//    gl_Position = vec4(inPos.xy, 0.0, 1.0);
}