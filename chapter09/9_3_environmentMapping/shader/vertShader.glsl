#version 430
// 指定 GLSL 的版本为 4.30

layout (location=0) in vec3 position;
layout (location=1) in vec2 texCoord; // 输入变量，表示顶点的颜色，绑定到 location = 1
layout (location=2) in vec3 normal; // 输入变量，表示顶点的法线，绑定到 location = 2
// 输入变量，表示顶点的三维位置，绑定到 location = 0

uniform mat4 mv_matrix;
// uniform 变量，表示模型-视图矩阵，用于将顶点从模型空间变换到视图空间

uniform mat4 proj_matrix;
// uniform 变量，表示投影矩阵，用于将顶点从视图空间变换到裁剪空间

uniform mat4 normal_matrix;
out vec2 tc;

out vec3 fragNormal;

out vec3 vertPos; 
void main(void)
// 主函数，计算顶点的最终位置
{
    vertPos=(mv_matrix*vec4(position,1.0)).xyz;
    gl_Position = proj_matrix * mv_matrix * vec4(position,1.0);
    // 将顶点位置从模型空间依次变换到视图空间和裁剪空间
    // 最终结果存储在内置变量 gl_Position 中，用于后续的光栅化阶段
    tc = texCoord;

    fragNormal = mat3(normal_matrix) * normal;
    // 将法线从模型空间变换到视图空间
    
}
