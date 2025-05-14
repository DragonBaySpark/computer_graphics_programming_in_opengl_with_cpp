#version 430

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

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex_coord;
layout(location = 2) in vec3 vertNormal;
out vec2 tc;

out vec3 varyingNormal;
out vec3 varyingLightDir;
out vec3 varyingVertPos;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;

uniform vec4 globalAmbient;      // 全局环境光

uniform PositionalLight light;    // 点光源属性
uniform Material material;        // 物体材质属性

uniform mat4 shadowMVP;          // 光源变换矩阵
layout(binding = 0) uniform sampler2DShadow s;

out vec4 shadow_coord;

void main(void) {

    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);

    varyingVertPos = (mv_matrix * vec4(position, 1.0)).xyz;
    varyingNormal = (norm_matrix * vec4(vertNormal, 1.0)).xyz;
    varyingLightDir = light.position - varyingVertPos;

    tc = tex_coord;

    shadow_coord = shadowMVP * vec4(position, 1.0);
}