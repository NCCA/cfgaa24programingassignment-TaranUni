#version 410 core

layout (location =0) out vec4 fragColour;

in vec3 worldPos;
in vec3 normal;

void main()
{
    fragColour=vec4(0.3f,0.2f,0.1f,1.0f);
}