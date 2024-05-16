/*
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in float size;

out vec3 fragColor;

uniform mat4 MVP;

void main()
{
    fragColor = color;
    gl_PointSize = size;
    gl_Position = MVP * vec4(position, 1.0);
}
*/

#version 410 core

layout (location=0) in vec3 inPos;
layout (location=1) in vec3 inColour;
out vec3 particleColour;

uniform mat4 MVP;

void main()
{
    gl_Position=MVP*vec4(inPos,1);
    particleColour=vec3(1,0,0);
}


//// this demo is based on code from here https://learnopengl.com/#!PBR/Lighting
///// @brief the vertex passed in
//layout (location = 0) in vec3 inVert;
///// @brief the normal passed in
//layout (location = 1) in vec3 inNormal;
///// @brief the in uv
//layout (location = 2) in vec2 inUV;
//
//out vec3 worldPos;
//out vec3 normal;
//
//layout( std140) uniform TransformUBO
//{
//    mat4 MVP;
//    mat4 normalMatrix;
//    mat4 M;
//}transforms;
//
//void main()
//{
//    //    gl_Position=MVP*vec4(inPos.xyz,1);
//    //    gl_PointSize=inPos.w;
//    //    objectColour=inColour;
//
//    //    objectColour=vec3(1,1,1);
//    //    objectColour = inColour;
//    //    gl_Position = vec4(inPos.xy, 0.0, 1.0);
//
//    worldPos = vec3(transforms.M * vec4(inVert, 1.0f));
//    normal=normalize(mat3(transforms.normalMatrix)*inNormal);
//    gl_Position = MVP*vec4(inVert,1.0);
//}