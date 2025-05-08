#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp> // glm::value_ptr
#include <glm\gtc\matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Utils.h"
using namespace std;

#define numVAOs 1 // 定义顶点数组对象（VAO）的数量
#define numVBOs 2 // 定义顶点缓冲区对象（VBO）的数量

Utils util = Utils();
float cameraX, cameraY, cameraZ; // 摄像机位置
float cubeLocX, cubeLocY, cubeLocZ; // 立方体位置
GLuint renderingProgram; // 着色器程序
GLuint vao[numVAOs]; // VAO 数组
GLuint vbo[numVBOs]; // VBO 数组

// 变量分配用于显示
GLuint mvLoc, projLoc; // 模型-视图矩阵和投影矩阵的 uniform 位置
int width, height; // 窗口宽度和高度
float aspect; // 视口宽高比
glm::mat4 pMat, vMat, mMat, mvMat; // 投影矩阵、视图矩阵、模型矩阵和模型-视图矩阵

// 设置顶点数据
void setupVertices(void) {
    float vertexPositions[3*6] = {
        // 三角形的顶点坐标,颜色
        -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    };

    glGenVertexArrays(1, vao); // 生成 VAO
    glBindVertexArray(vao[0]); // 绑定 VAO
    glGenBuffers(numVBOs, vbo); // 生成 VBO

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); // 绑定 VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW); // 将顶点数据传递到 VBO


    //glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); // 绑定 VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)0);  // 设置顶点属性指针：坐标
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)(3 * sizeof(float)));  // 设置顶点属性指针：颜色
    glEnableVertexAttribArray(0); // 启用顶点属性
    glEnableVertexAttribArray(1); // 启用顶点属性
}

// 初始化函数
void init(GLFWwindow* window) {
    renderingProgram = Utils::createShaderProgram("./shader/vertShader.glsl", "./shader/fragShader.glsl");
    // 创建着色器程序
    cameraX = 0.0f; cameraY = 0.0f; cameraZ = 2.0f; // 初始化摄像机位置
    cubeLocX = 0.0f; cubeLocY = 0.0f; cubeLocZ = 0.0f; // 初始化立方体位置
    setupVertices(); // 设置顶点数据
}

// 显示函数
void display(GLFWwindow* window, double currentTime) {
    glClear(GL_DEPTH_BUFFER_BIT); // 清除深度缓冲区

    glUseProgram(renderingProgram); // 使用着色器程序

    // 获取 uniform 变量的位置
    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

    // 获取窗口尺寸并计算宽高比
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 设置投影矩阵

    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ)); // 设置视图矩阵
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(cubeLocX, cubeLocY, cubeLocZ)); // 设置模型矩阵
    mvMat = vMat * mMat; // 计算模型-视图矩阵

    // 将矩阵传递到着色器
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

    #if 0
    //这段代码在 setupVertices() 中已经设置过了，这里注释掉,不需要每一帧都设置一次
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); // 绑定 VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)0);  // 设置顶点属性指针：坐标
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)(3 * sizeof(float)));  // 设置顶点属性指针：颜色
    glEnableVertexAttribArray(0); // 启用顶点属性
    glEnableVertexAttribArray(1); // 启用顶点属性
    #endif 

    glEnable(GL_DEPTH_TEST); // 启用深度测试
    glDepthFunc(GL_LEQUAL); // 设置深度测试函数

    glDrawArrays(GL_TRIANGLES, 0, 3); // 绘制三角形
}

// 主函数
int main(void) {
    if (!glfwInit()) { exit(EXIT_FAILURE); } // 初始化 GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // 设置 OpenGL 主版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // 设置 OpenGL 次版本号
    GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter 4 - program 1", NULL, NULL); // 创建窗口
    glfwMakeContextCurrent(window); // 设置当前上下文
    if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); } // 初始化 GLEW
    glfwSwapInterval(1); // 开启垂直同步

    init(window); // 初始化

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        display(window, glfwGetTime()); // 调用显示函数
        glfwSwapBuffers(window); // 交换缓冲区
        glfwPollEvents(); // 处理事件
    }

    glfwDestroyWindow(window); // 销毁窗口
    glfwTerminate(); // 终止 GLFW
    exit(EXIT_SUCCESS); // 正常退出
}