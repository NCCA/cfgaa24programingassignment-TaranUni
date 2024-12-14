#version 410 core

layout (location=0) out vec4 fragColour;
in vec3 particleColour;

void main()
{
    fragColour.rgb=particleColour;
}