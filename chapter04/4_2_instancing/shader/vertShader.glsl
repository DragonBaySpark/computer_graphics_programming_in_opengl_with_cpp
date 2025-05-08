#version 430
// 指定 GLSL 的版本为 4.30

layout (location=0) in vec3 position;
// 输入变量，表示顶点的三维位置，绑定到 location = 0

// uniform 变量，表示模型-视图矩阵，用于将顶点从模型空间变换到视图空间
//uniform mat4 mv_matrix;
uniform mat4 m_matrix;
uniform mat4 v_matrix;

uniform mat4 proj_matrix;
// uniform 变量，表示投影矩阵，用于将顶点从视图空间变换到裁剪空间

uniform float tf; // uniform 变量，表示时间，用于动画效果
out vec4 v_color;

mat4 buildTranslate(float x, float y, float z)
{
    // 构建平移矩阵
    mat4 translate = mat4(1.0);
    translate[3][0] = x;
    translate[3][1] = y;
    translate[3][2] = z;
    return translate;
}

mat4 buildRotateX(float angle)
{
    // 构建旋转矩阵
    mat4 rotate =mat4(1.0,0.0,0.0,0.0,
                     0.0,cos(angle),-sin(angle),0.0,
                     0.0,sin(angle),cos(angle),0.0,
                     0.0,0.0,0.0,1.0);
    // 旋转矩阵的构建公式，使用了旋转角度和旋转轴的坐标
    return rotate;
}
mat4 buildRotateY(float angle)
{
    // 构建旋转矩阵
    mat4 rotate =mat4(cos(angle),0.0,sin(angle),0.0,
                     0.0,1.0,0.0,0.0,
                     -sin(angle),0.0,cos(angle),0.0,
                     0.0,0.0,0.0,1.0);
    return rotate;
}
mat4 buildRotateZ(float angle)
{
    // 构建旋转矩阵
    mat4 rotate =mat4(cos(angle),-sin(angle),0.0,0.0,
                     sin(angle),cos(angle),0.0,0.0,
                     0.0,0.0,1.0,0.0,
                     0.0,0.0,0.0,1.0);
    return rotate;
}


void main(void)
// 主函数，计算顶点的最终位置
{
    float i=gl_InstanceID;
    // 获取当前实例的 ID，用于实例化渲染
    mat4 rotateX = buildRotateX( 1* 1.75f*tf);
    mat4 rotateY = buildRotateY( 1 * 1.75f*tf);
    mat4 rotateZ = buildRotateZ( 1 * 1.75f*tf);
    mat4 rotate = rotateX * rotateY * rotateZ;
    // 通过平移、旋转矩阵构建最终的旋转矩阵
    float x = float(i * 5.0f);
    float y = float(i * 2.5f);
    float z = float(i * 2.5f);

    mat4 matrix =buildTranslate(-8,-2,0)* buildTranslate(x, y, -z)*rotate;

    //mat4 mv_matrix = v_matrix * m_matrix * translate;
    mat4 mv_matrix = v_matrix * matrix;
    // 计算模型-视图矩阵，用于将顶点从模型空间变换到视图空间
    gl_Position = proj_matrix * mv_matrix * vec4(position,1.0);
    // 将顶点位置从模型空间依次变换到视图空间和裁剪空间
    // 最终结果存储在内置变量 gl_Position 中，用于后续的光栅化阶段
    v_color = vec4(position,1.0)*0.5+vec4(0.5,0.5,0.5,0.5);
}
