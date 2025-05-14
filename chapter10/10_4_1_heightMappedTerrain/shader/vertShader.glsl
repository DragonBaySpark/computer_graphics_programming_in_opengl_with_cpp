// 指定GLSL版本为4.30
#version 430

// 顶点属性输入
layout(location = 0) in vec3 vertPos;    // 顶点位置
layout(location = 1) in vec2 vertTexCoord; // 顶点纹理坐标
layout(location = 2) in vec3 vertNormal; // 顶点法线

// 输出到片段着色器的变量
out vec3 varyingNormal;    // 变换后的法线
out vec3 varyingLightDir;  // 光源方向
out vec3 varyingVertPos;   // 视图空间中的顶点位置
out vec3 originalVertPos;
out vec2 varyingTexCoord; // 变换后的纹理坐标

// 点光源结构体定义
struct PositionalLight {
    vec4 ambient;    // 环境光分量
    vec4 diffuse;    // 漫反射分量
    vec4 specular;   // 镜面反射分量
    vec3 position;   // 光源位置
};

// 材质结构体定义
struct Material {
    vec4 ambient;    // 材质的环境光反射系数
    vec4 diffuse;    // 材质的漫反射系数
    vec4 specular;   // 材质的镜面反射系数
    float shininess; // 镜面反射的光泽度
};

// uniform变量声明
uniform vec4 globalAmbient;      // 全局环境光
uniform PositionalLight light;    // 点光源属性
uniform Material material;        // 物体材质属性
uniform mat4 mv_matrix;          // 模型视图矩阵
uniform mat4 proj_matrix;        // 投影矩阵
uniform mat4 norm_matrix;        // 法线变换矩阵

layout(binding = 0) uniform sampler2D heightMap;    // 高度贴图

void main(void) {
    vec3 p = vertPos + vertNormal * texture(heightMap, vertTexCoord).r * 0.2;
    // 计算顶点在视图空间中的位置
    varyingVertPos = (mv_matrix * vec4(p, 1.0)).xyz;
    originalVertPos = vertPos;

    // 计算从顶点到光源的方向向量
    varyingLightDir = light.position - varyingVertPos;

    // 变换法线到视图空间
    // 使用法线矩阵以正确处理非统一缩放
    varyingNormal = (norm_matrix * vec4(vertNormal, 1.0)).xyz;

    varyingTexCoord = vertTexCoord; // 传递纹理坐标

    // 计算裁剪空间中的顶点位置
    gl_Position = proj_matrix * mv_matrix * vec4(p, 1.0);
}