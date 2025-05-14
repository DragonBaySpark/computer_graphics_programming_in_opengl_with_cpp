// 指定GLSL版本为4.30
#version 430

// 从顶点着色器接收的插值数据
in vec3 varyingNormal;    // 插值后的法线向量
in vec3 varyingLightDir;  // 插值后的光源方向
in vec3 varyingVertPos;   // 插值后的顶点位置
in vec3 originalVertPos; // 原始顶点位置
in vec3 varyingTangent; // 插值后的切线
in vec2 varyingTexCoord; // 插值后的纹理坐标


// 输出的片段颜色
out vec4 fragColor;

// 点光源结构体定义
struct PositionalLight
{   
    vec4 ambient;    // 环境光分量
    vec4 diffuse;    // 漫反射分量
    vec4 specular;   // 镜面反射分量
    vec3 position;   // 光源位置
};

// 材质结构体定义
struct Material
{   
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

layout (binding =0) uniform sampler2D normalMap;    // 法线贴图
//uniform sampler2D heightMap;    // 高度贴图


// 计算法线的函数
// normal: 顶点的法线向量
// tangent: 顶点的切线向量
// texCoord: 纹理坐标
vec3 calcNormal(vec3 normal, vec3 tangent, vec2 texCoord)
{
    // 使用Gram-Schmidt正交化计算切线T
    // 确保切线与法线垂直
    vec3 T = normalize(tangent - dot(tangent, normal) * normal);
    
    // 通过叉积计算副切线B
    // T和N叉积得到B，确保三个向量相互垂直
    vec3 B = normalize(cross(T, normal));
    
    // 从法线贴图中获取法线数据并转换到[-1,1]范围
    vec3 N = normalize(texture(normalMap, texCoord).xyz * 2.0 - 1.0);
    
    // 构建TBN矩阵
    // 用于将切线空间的法线转换到世界空间
    mat3 TBN = mat3(T, B, N);
    
    // 从法线贴图中获取法线数据
    vec3 retrivedNormal = texture(normalMap, texCoord).xyz;
    
    // 将法线从[0,1]范围转换到[-1,1]范围
    retrivedNormal = normalize(retrivedNormal * 2.0 - 1.0);
    
    // 使用TBN矩阵将法线从切线空间转换到世界空间
    vec3 newNormal = normalize(TBN * retrivedNormal);
    
    return newNormal;
}

void main(void)
{   
    // 标准化光照计算所需的向量
    vec3 L = normalize(varyingLightDir);    // 光源方向向量
    //vec3 N = normalize(varyingNormal);      // 表面法线向量

    float x=originalVertPos.x;
    float y=originalVertPos.y;  
    float z=originalVertPos.z;

    /**
    采用过程式凹凸贴图
    */

/*
    float a=0.25f;
    float b=100;
    vec3 N=normalize(varyingNormal+vec3(a*sin(b*x),a*sin(b*y),a*sin(b*z)));
*/
    //采用法线贴图
    vec3 N=calcNormal(varyingNormal, varyingTangent, varyingTexCoord);

    vec3 V = normalize(-varyingVertPos);    // 视线向量（指向相机）
    
    // 计算反射向量R
    // reflect(-L, N)计算-L关于N的反射方向
    vec3 R = normalize(reflect(-L, N));
    
    // 计算光源方向与法线的夹角余弦值
    // 用于漫反射计算
    float cosTheta = dot(L,N);
    
    // 计算视线方向与反射光线的夹角余弦值
    // 用于镜面反射计算
    float cosPhi = dot(V,R);

    // 计算Phong光照模型的三个分量（每个像素）
    
    // 1. 环境光分量：考虑全局环境光和光源的环境光
    vec3 ambient = ((globalAmbient * material.ambient) + 
                    (light.ambient * material.ambient)).xyz;
    
    // 2. 漫反射分量：与表面法线和光源方向的夹角有关
    vec3 diffuse = light.diffuse.xyz * material.diffuse.xyz * 
                   max(cosTheta,0.0);
    
    // 3. 镜面反射分量：与反射向量和视线方向的夹角有关
    vec3 specular = light.specular.xyz * material.specular.xyz * 
                    pow(max(cosPhi,0.0), material.shininess);
    
    // 将三个分量相加得到最终颜色
    fragColor = vec4((ambient + diffuse + specular), 1.0);
}