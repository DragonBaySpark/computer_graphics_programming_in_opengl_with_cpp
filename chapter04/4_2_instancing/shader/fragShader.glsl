#version 430
// 指定 GLSL 的版本为 4.30

in vec4 v_color;
out vec4 color;
// 输出变量，表示片段的最终颜色

uniform mat4 mv_matrix;
// uniform 变量，模型-视图矩阵（未使用）

uniform mat4 proj_matrix;
// uniform 变量，投影矩阵（未使用）

void main(void)
// 主函数，计算片段的最终颜色
{
    color=v_color;
}
