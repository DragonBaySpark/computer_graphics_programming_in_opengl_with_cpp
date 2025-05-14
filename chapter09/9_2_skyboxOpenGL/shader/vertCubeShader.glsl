
#version 430
// 指定 GLSL 的版本为 4.30
layout (location=0) in vec3 position; // 输入变量，表示顶点的三维位置，绑定到 location = 0


out vec3 texCoord; // 输出变量

uniform mat4 mv_matrix;
// uniform 变量，表示模型-视图矩阵，用于将顶点从模型空间变换到视图空间
uniform mat4 proj_matrix;
// uniform 变量，表示投影矩阵，用于将顶点从视图空间变换到裁剪空间
void main(void)
// 主函数，计算顶点的最终位置
{
    mat4 vrot_matrix=mat4(mat3(mv_matrix)); //remove the translation part
    gl_Position = proj_matrix * vrot_matrix * vec4(position,1.0);
    // 将顶点位置从模型空间依次变换到视图空间和裁剪空间
    // 最终结果存储在内置变量 gl_Position 中，用于后续的光栅化阶段
    texCoord = position;
}