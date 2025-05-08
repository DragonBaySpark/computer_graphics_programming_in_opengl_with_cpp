#version 430
// 指定 GLSL 的版本为 4.30

in vec3 v_color;
// 输入变量，表示顶点的颜色（未使用）
out vec4 color;
// 输出变量，表示片段的最终颜色

uniform mat4 mv_matrix;
// uniform 变量，模型-视图矩阵（未使用）

uniform mat4 proj_matrix;
// uniform 变量，投影矩阵（未使用）

void main(void)
// 主函数，计算片段的最终颜色
{
    color = vec4(v_color, 1.0);
    // 将片段颜色设置为红色，RGBA 格式，其中 R=1.0，G=0.0，B=0.0，A=1.0（完全不透明）
}
