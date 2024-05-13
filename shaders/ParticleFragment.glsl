#version 410 core

layout (location=0) out vec4 fragColour;
in vec3 particleColour;

const float PI = 3.1415926;

float SmoothingKernelPoly6(float dst, float radius)
{
    if (dst < radius)
    {
        float scale = 315 / (64 * PI * pow(abs(radius), 9));
        float v = radius * radius - dst * dst;
        return v * v * v * scale;
    }
    return 0;
}

float DensityKernel(float dst, float radius)
{
    return SmoothingKernelPoly6(dst, radius);
}

void main()
{
    fragColour.rgb=particleColour;
}