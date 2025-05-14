/*
* 第8章 阴影映射示例程序
* 这个程序演示了如何使用OpenGL实现阴影映射技术
* 包含了深度纹理的创建、阴影贴图的生成和渲染
*/

// OpenGL相关库
#include <GL\glew.h>      // OpenGL扩展加载库
#include <GLFW\glfw3.h>   // 窗口和输入管理库
#include <SOIL2\soil2.h>  // 图像加载库

// 标准库
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

// GLM数学库
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>			// 提供glm::value_ptr函数
#include <glm\gtc\matrix_transform.hpp> // 提供矩阵变换函数

// 自定义类
#include "ImportedModel.h"  // 导入模型类
#include "Utils.h"          // 工具函数
#include "Torus.h"          // 圆环模型类

using namespace std;

// 顶点数组对象和顶点缓冲对象的数量
#define numVAOs 1
#define numVBOs 7

// 相机位置
float cameraX, cameraY, cameraZ;
// 物体位置
float objLocX, objLocY, objLocZ;

// 着色器程序
GLuint renderingProgram;         // 主渲染着色器程序
GLuint renderingProgramShadow;   // 阴影映射着色器程序

// 顶点数组和缓冲对象
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint shuttleTexture;           // 模型纹理

// 着色器统一变量位置
GLuint mvLoc, projLoc, nLoc;     // 矩阵变换相关
// 光照和材质相关的统一变量位置
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mambLoc, mdiffLoc, mspecLoc, mshiLoc;

// 窗口和视口相关
int width, height;
float aspect;                    // 宽高比

// 变换矩阵
glm::mat4 pMat;                  // 投影矩阵
glm::mat4 vMat;                  // 视图矩阵
glm::mat4 mMat;                  // 模型矩阵
glm::mat4 mvMat;                 // 模型-视图矩阵
glm::mat4 invTrMat;              // 法线变换矩阵(逆转置矩阵)
glm::mat4 rMat;                  // 旋转矩阵
glm::vec3 transformed;           // 变换后的光源位置
float lightPos[3];               // 光源位置数组(用于着色器)

// 模型对象
ImportedModel myModel("./texture/pyr.obj");  // 导入的金字塔模型
Torus myTorus(0.5f, 0.2f, 48);               // 圆环模型(主半径0.5，管半径0.2，48个分段)

// 光照属性 - 白光
float globalAmbient[4] = {0.7f, 0.7f, 0.7f, 1.0f};  // 全局环境光
float lightAmbient[4] = {0.0f, 0.0f, 0.0f, 1.0f};   // 光源环境光分量
float lightDiffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};   // 光源漫反射分量
float lightSpecular[4] = {1.0f, 1.0f, 1.0f, 1.0f};  // 光源镜面反射分量

// 材质属性 - 金色
float *matAmb = Utils::goldAmbient();     // 材质环境光反射系数
float *matDif = Utils::goldDiffuse();     // 材质漫反射系数
float *matSpe = Utils::goldSpecular();    // 材质镜面反射系数
float matShi = Utils::goldShininess();    // 材质光泽度

// 圆环模型索引数量
int numTorusIndices = 0;

// 阴影映射相关变量
int screenSizeX, screenSizeY;             // 屏幕尺寸
GLuint shadowTex, shadowBuffer;           // 阴影贴图纹理和帧缓冲对象
glm::mat4 lightVmatrix;                   // 光源视图矩阵
glm::mat4 lightPmatrix;                   // 光源投影矩阵
glm::mat4 shadowMVP1;                     // 阴影映射第一阶段MVP矩阵
glm::mat4 shadowMVP2;                     // 阴影映射第二阶段MVP矩阵
glm::mat4 b;                              // 偏置矩阵，用于将NDC坐标转换到[0,1]范围

// 光源和视点位置
glm::vec3 lightLoc(-3.8f, 2.2f, 0.1f);    // 光源位置
glm::vec3 currentLightPos;                // 当前光源位置(可能会随时间变化)
glm::vec3 origin(-0.5f, 0.0f, 0.2f);      // 视点位置(光源看向的点)
glm::vec3 up(0.0f, 1.0f, 0.0f);           // 上方向向量

float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

void setupShadowBuffers(GLFWwindow *window)
{
	glfwGetFramebufferSize(window, &screenSizeX, &screenSizeY);
	width = screenSizeX;
	height = screenSizeY;

	glGenFramebuffers(1, &shadowBuffer);
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, screenSizeX, screenSizeY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
}
void setupVertices(void)
{
	std::vector<glm::vec3> vert = myModel.getVertices();
	std::vector<glm::vec2> tex = myModel.getTextureCoords();
	std::vector<glm::vec3> norm = myModel.getNormals();

	std::vector<float> pvalues;
	std::vector<float> tvalues;
	std::vector<float> nvalues;

	for (int i = 0; i < myModel.getNumVertices(); i++)
	{
		pvalues.push_back((vert[i]).x);
		pvalues.push_back((vert[i]).y);
		pvalues.push_back((vert[i]).z);
		tvalues.push_back((tex[i]).s);
		tvalues.push_back((tex[i]).t);
		nvalues.push_back((norm[i]).x);
		nvalues.push_back((norm[i]).y);
		nvalues.push_back((norm[i]).z);
	}

	std::vector<glm::vec3> vert_torus = myTorus.getVertices();
	std::vector<glm::vec2> tex_torus = myTorus.getTexCoords();
	std::vector<glm::vec3> norm_torus = myTorus.getNormals();

	std::vector<float> pvalues_torus;
	std::vector<float> tvalues_torus;
	std::vector<float> nvalues_torus;

	for (int i = 0; i < myTorus.getNumVertices(); i++)
	{
		pvalues_torus.push_back(vert_torus[i].x);
		pvalues_torus.push_back(vert_torus[i].y);
		pvalues_torus.push_back(vert_torus[i].z);
		tvalues_torus.push_back(tex_torus[i].s);
		tvalues_torus.push_back(tex_torus[i].t);
		nvalues_torus.push_back(norm_torus[i].x);
		nvalues_torus.push_back(norm_torus[i].y);
		nvalues_torus.push_back(norm_torus[i].z);
	}

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, pvalues_torus.size() * 4, &pvalues_torus[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, tvalues_torus.size() * 4, &tvalues_torus[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, nvalues_torus.size() * 4, &nvalues_torus[0], GL_STATIC_DRAW);

	std::vector<int> ind = myTorus.getIndices();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[6]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0], GL_STATIC_DRAW);

	numTorusIndices = ind.size();
}
void installLights(glm::mat4 vMatrix)
{
	transformed = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
	lightPos[0] = transformed.x;
	lightPos[1] = transformed.y;
	lightPos[2] = transformed.z;

	// get the locations of the light and material fields in the shader
	globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmbient");
	ambLoc = glGetUniformLocation(renderingProgram, "light.ambient");
	diffLoc = glGetUniformLocation(renderingProgram, "light.diffuse");
	specLoc = glGetUniformLocation(renderingProgram, "light.specular");
	posLoc = glGetUniformLocation(renderingProgram, "light.position");
	mambLoc = glGetUniformLocation(renderingProgram, "material.ambient");
	mdiffLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
	mspecLoc = glGetUniformLocation(renderingProgram, "material.specular");
	mshiLoc = glGetUniformLocation(renderingProgram, "material.shininess");

	//  set the uniform light and material values in the shader
	glProgramUniform4fv(renderingProgram, globalAmbLoc, 1, globalAmbient);
	glProgramUniform4fv(renderingProgram, ambLoc, 1, lightAmbient);
	glProgramUniform4fv(renderingProgram, diffLoc, 1, lightDiffuse);
	glProgramUniform4fv(renderingProgram, specLoc, 1, lightSpecular);
	glProgramUniform3fv(renderingProgram, posLoc, 1, lightPos);
	glProgramUniform4fv(renderingProgram, mambLoc, 1, matAmb);
	glProgramUniform4fv(renderingProgram, mdiffLoc, 1, matDif);
	glProgramUniform4fv(renderingProgram, mspecLoc, 1, matSpe);
	glProgramUniform1f(renderingProgram, mshiLoc, matShi);
}
void init(GLFWwindow *window)
{
	renderingProgram = Utils::createShaderProgram("./shader/vertShader.glsl", "./shader/fragShader.glsl");
	renderingProgramShadow = Utils::createShaderProgram("./shader/shadowVertShader.glsl", "./shader/shadowFragShader.glsl");
	cameraX = 0.0f;
	cameraY = 0.0f;
	cameraZ = 2.5f;
	// objLocX = -0.1f; objLocY = 0.0f; objLocZ =-0.5f;
	objLocX = -0.6f;
	objLocY = 0.3f;
	objLocZ = 0.2f;

	currentLightPos = glm::vec3(lightLoc);
	lightVmatrix = glm::lookAt(currentLightPos, origin, up);
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	setupVertices();
	shuttleTexture = Utils::loadTexture("./texture/Dolphin_HighPolyUV.png");

	setupShadowBuffers(window);
	b = glm::mat4(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0);

	// lightVmatrix = glm::lookAt(glm::vec3(lightPos[0], lightPos[1], lightPos[2]), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	// lightPmatrix = glm::perspective(1.0472f, 1.0f, 0.1f, 1000.0f);
}


/**
 * 阴影映射的第一个渲染阶段(Pass One)
 * 从光源视角渲染场景，生成深度贴图(阴影贴图)
 * 
 * @param window GLFW窗口指针
 * @param currentTime 当前时间，用于动画
 */
void passOne(GLFWwindow *window, double currentTime)
{
	// 使用阴影映射专用的着色器程序
	glUseProgram(renderingProgramShadow);

	// 使用光源视图矩阵作为当前视图矩阵
	// 这样我们就能从光源的视角渲染场景
	vMat = lightVmatrix;
	float angle = currentTime * 0.5f;
	float r = 1.0f;

	// 注释掉的代码是用于物体旋转的
	// objLocX = r * glm::cos(angle);
	// objLocZ = r * glm::sin(angle);
	// mMat = glm::translate(glm::mat4(1.0f), glm::vec3(objLocX, objLocY, objLocZ)) * glm::rotate(glm::mat4(1.0f), toRadians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// 设置金字塔模型的模型矩阵：平移、旋转和缩放
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(objLocX, objLocY, objLocZ)) * 
	       glm::rotate(glm::mat4(1.0f), toRadians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * 
	       glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));

	// 计算阴影映射的MVP矩阵(从光源视角)
	// 这个矩阵将用于将顶点从模型空间变换到光源的裁剪空间
	shadowMVP1 = lightPmatrix * lightVmatrix * mMat;

	// 计算模型-视图矩阵
	mvMat = vMat * mMat;

	// 将阴影MVP矩阵传递给着色器
	GLuint shadowMVP1Loc = glGetUniformLocation(renderingProgramShadow, "shadowMVP");
	glUniformMatrix4fv(shadowMVP1Loc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

	// 清除深度缓冲区并设置OpenGL状态
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);      // 启用深度测试
	glEnable(GL_CULL_FACE);       // 启用面剔除
	glFrontFace(GL_CCW);          // 设置逆时针为正面
	glDepthFunc(GL_LEQUAL);       // 设置深度测试函数

	// 设置金字塔模型的顶点属性
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// 绘制金字塔模型
	glDrawArrays(GL_TRIANGLES, 0, myModel.getNumVertices());

	// 绘制圆环模型
	
	// 设置圆环模型的模型矩阵
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 0.2f)) * 
	       glm::rotate(glm::mat4(1.0f), toRadians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * 
	       glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
	
	// 更新圆环的阴影MVP矩阵
	shadowMVP1 = lightPmatrix * lightVmatrix * mMat;
	shadowMVP1Loc = glGetUniformLocation(renderingProgramShadow, "shadowMVP");
	glUniformMatrix4fv(shadowMVP1Loc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

	// 设置OpenGL状态
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// 设置圆环模型的顶点属性
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// 使用索引绘制圆环模型
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[6]);
	glDrawElements(GL_TRIANGLES, numTorusIndices, GL_UNSIGNED_INT, 0);
}

/**
 * 阴影映射的第二个渲染阶段(Pass Two)
 * 从相机视角渲染场景，并应用阴影贴图实现阴影效果
 * 
 * @param window GLFW窗口指针
 * @param currentTime 当前时间，用于动画
 */
void passTwo(GLFWwindow *window, double currentTime)
{
	// 使用主渲染着色器程序
	glUseProgram(renderingProgram);
	
	// 获取着色器中矩阵变量的位置
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");    // 模型-视图矩阵
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix"); // 投影矩阵
	nLoc = glGetUniformLocation(renderingProgram, "norm_matrix");    // 法线变换矩阵

	// 创建相机视图矩阵
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

	float angle = currentTime * 0.5f;
	float r = 1.0f;

	// 注释掉的代码是用于物体旋转的
	// objLocX = r * glm::cos(angle);
	// objLocZ = r * glm::sin(angle);
	// mMat = glm::translate(glm::mat4(1.0f), glm::vec3(objLocX, objLocY, objLocZ)) * glm::rotate(glm::mat4(1.0f), toRadians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	// 设置金字塔模型的模型矩阵
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(objLocX, objLocY, objLocZ)) * 
	       glm::rotate(glm::mat4(1.0f), toRadians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * 
	       glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));

	// 设置光照参数
	installLights(vMat);

	// 计算阴影映射矩阵
	// b是偏置矩阵，将NDC坐标[-1,1]转换到纹理坐标[0,1]范围
	// 这个矩阵用于将顶点位置变换到光源空间，以便进行阴影测试
	shadowMVP2 = b * lightPmatrix * lightVmatrix * mMat;

	// 计算模型-视图矩阵和法线变换矩阵
	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat)); // 法线矩阵是模型-视图矩阵的逆转置

	// 将阴影MVP矩阵传递给着色器
	GLuint shadowMVP2Loc = glGetUniformLocation(renderingProgram, "shadowMVP");
	glUniformMatrix4fv(shadowMVP2Loc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));

	// 将其他矩阵传递给着色器
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	// 清除深度缓冲并设置OpenGL状态
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// 设置金字塔模型的顶点属性
	// 位置属性
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// 纹理坐标属性
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// 法线属性
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// 绑定阴影贴图纹理
	// 这个纹理包含了从光源视角看到的深度信息
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	// 确保深度测试已启用
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// 绘制金字塔模型
	glDrawArrays(GL_TRIANGLES, 0, myModel.getNumVertices());

	// 绘制圆环模型
	
	// 设置圆环模型的模型矩阵
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 0.2f)) * 
	       glm::rotate(glm::mat4(1.0f), toRadians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * 
	       glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));

	// 更新模型-视图矩阵并传递给着色器
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	// 设置圆环模型的顶点属性
	// 位置属性
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// 纹理坐标属性
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// 法线属性
	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// 设置OpenGL状态
	glEnable(GL_CULL_FACE);       // 启用面剔除
	glFrontFace(GL_CCW);          // 设置逆时针为正面
	glEnable(GL_DEPTH_TEST);      // 启用深度测试
	glDepthFunc(GL_LEQUAL);       // 设置深度测试函数

	// 使用索引绘制圆环模型
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[6]);
	glDrawElements(GL_TRIANGLES, numTorusIndices, GL_UNSIGNED_INT, 0);
}

void saveShadowMapToFile(const char *filename, GLuint shadowTexture, int width, int height)
{
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	GLfloat *data = new GLfloat[width * height];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);

	unsigned char *data2 = new unsigned char[width * height * 3];
	for (int i = 0; i < width * height; i++)
	{
		data2[i * 3 + 0] = (unsigned char)(data[i] * 255);
		data2[i * 3 + 1] = (unsigned char)(data[i] * 255);
		data2[i * 3 + 2] = (unsigned char)(data[i] * 255);
	}
	int success = SOIL_save_image(filename, SOIL_SAVE_TYPE_BMP, width, height, 3, data2);
	if (success == 0)
	{
		std::cout << "Error saving shadow map to file: " << SOIL_last_result() << std::endl;
	}
	else
	{
		std::cout << "Shadow map saved to file: " << filename << std::endl;
	}
	delete[] data;
}
void display(GLFWwindow *window, double currentTime)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	currentLightPos = glm::vec3(lightLoc);

	lightVmatrix = glm::lookAt(currentLightPos, origin, up);
	lightPmatrix = glm::perspective(toRadians(60.0f), aspect, 0.1f, 1000.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
	// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glEnable(GL_DEPTH_TEST);

	//减少阴影痤疮
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.0f, 4.0f);

	// 绘制场景
	passOne(window, currentTime);

	static int count = 0;
	if (count == 0)
	{
		saveShadowMapToFile("shadowMap.bmp", shadowTex, screenSizeX, screenSizeY);
		count++;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glDrawBuffer(GL_FRONT);
	passTwo(window, currentTime);
}

void window_size_callback(GLFWwindow *win, int newWidth, int newHeight)
{
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

int main(void)
{
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow *window = glfwCreateWindow(1000, 1000, "Chapter6 - program1", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK)
	{
		exit(EXIT_FAILURE);
	}
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);

	init(window);

	while (!glfwWindowShouldClose(window))
	{
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}