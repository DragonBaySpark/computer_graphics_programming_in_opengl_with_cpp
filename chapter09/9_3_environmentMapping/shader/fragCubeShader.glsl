#version 430
// 指定 GLSL 的版本为 4.30

in vec3 texCoord; // 输入变量，表示顶点对应的纹理坐标
out vec4 fragColor; // 输出变量，表示片元最终的颜色
uniform samplerCube texCube; // 纹理采样器，表示立方体贴图
void main(void)
{
    fragColor = texture(texCube, texCoord); // 采样立方体贴图，得到片元的颜色
}