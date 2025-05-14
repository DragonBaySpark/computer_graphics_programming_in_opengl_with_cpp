#version 430

layout (location =0 ) in vec3 position;

uniform mat4 shadowMVP; // 变换矩阵
void main()
{
    gl_Position = shadowMVP*vec4(position,1.0);
}