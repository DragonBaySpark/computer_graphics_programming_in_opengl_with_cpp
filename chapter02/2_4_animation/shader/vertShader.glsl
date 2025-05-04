// 指定GLSL版本为4.30
#version 430

uniform float offsetX; // x轴偏移量
/**
 * 顶点着色器的主函数
 * 负责设置顶点的位置
 */
void main(void)
{
    // 设置顶点位置
    // vec4参数分别表示：
    // x=0.0：在x轴上居中
    // y=0.0：在y轴上居中
    // z=0.5：在z轴上的深度值
    // w=1.0：齐次坐标的w分量
    //gl_Position = vec4(0.0, 0.0, 0.5, 1.0);
    if (gl_VertexID == 0)
    {
        gl_Position = vec4(-0.3+offsetX, 0.0, 0.5, 1.0);
    }
    else if (gl_VertexID == 1)
    {
        gl_Position = vec4(0.2+offsetX, 0.0, 0.5, 1.0);
    }
    else if (gl_VertexID == 2)
    {
        gl_Position = vec4(0.2+offsetX, 0.5, 0.5, 1.0);
    }
    
}