#version 410 core

layout (location=0) out vec4 fragColour;
in vec3 objectColour;

void main()
{
    fragColour.rgb=vec3(1,1,1);
}