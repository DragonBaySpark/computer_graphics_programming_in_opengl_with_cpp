#version 430
// 指定 GLSL 的版本为 4.30

in vec2 tc;

in vec3 fragNormal;
in vec3 vertPos;
out vec4 color;
// 输出变量，表示片段的最终颜色

uniform mat4 mv_matrix;
// uniform 变量，模型-视图矩阵（未使用）

uniform mat4 proj_matrix;
// uniform 变量，投影矩阵（未使用）
layout (binding=0) uniform samplerCube tex0;

void main(void)
// 主函数，计算片段的最终颜色
{
    vec3 N = normalize(fragNormal);
    vec3 V = normalize(-vertPos); // 视线方向
    vec3 R = -reflect(V, N); // 反射方向
    
    color = texture(tex0, R);
    // 采样环境贴图，获取反射颜色

    //color=vec4(R,1.0); // 仅用于调试，显示反射方向
}
