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

in vec3 varyingNormal;
in vec3 varyingLightDir;
in vec3 varyingVertPos;

in vec2 tc;
in vec4 shadow_coord; // 传入的阴影坐标
out vec4 color;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

uniform vec4 globalAmbient;      // 全局环境光

uniform PositionalLight light;    // 点光源属性
uniform Material material;        // 物体材质属性

uniform mat4 norm_matrix;        // 法线变换矩阵
uniform mat4 shadowMVP;          // 光源变换矩阵
layout(binding = 0) uniform sampler2DShadow s;

void main(void) {
	//vec4 texColor = texture(s,tc);

	// 标准化光照计算所需的向量
	vec3 N = normalize(varyingNormal);
	vec3 L = normalize(varyingLightDir);
	vec3 V = normalize(-varyingVertPos);

	// 计算漫反射分量
	float lambertTerm = max(0, dot(N, L));
	vec4 Id = light.diffuse * material.diffuse * lambertTerm;

	// 计算镜面反射分量
	//vec3 R = reflect(-L,N);
	vec3 halfVector = normalize(L + V);

	float specular = pow(max(dot(N, halfVector), 0), material.shininess);
	vec4 Is = light.specular * material.specular * specular;

	// 计算环境光分量
	vec4 Ia = globalAmbient * material.ambient;

	// 计算最终颜色值 //采用纹理贴图
	//color = texColor*(globalAmbient+light.ambient + light.diffuse*(max(dot(N,L),0.0f))) +light.specular*pow(max(dot(N,halfVector),0.0f),material.shininess);

	float notInShadow = 1.0;
	notInShadow = textureProj(s, shadow_coord);
	//采用物体材质
	if(notInShadow > 0.50) {
		color = Ia + Id + Is;
	} else {

		color = Ia;
		//color=Ia+Id+Is;
	}

}
