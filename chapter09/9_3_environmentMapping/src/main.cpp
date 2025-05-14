#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>         // glm::value_ptr
#include <glm\gtc\matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Utils.h"
#include "Torus.h"
using namespace std;

#define numVAOs 1 // 定义顶点数组对象（VAO）的数量
#define numVBOs 4 // 定义顶点缓冲区对象（VBO）的数量

Utils util = Utils();
float cameraX, cameraY, cameraZ;    // 摄像机位置
float cubeLocX, cubeLocY, cubeLocZ; // 立方体位置
GLuint renderingProgram;            // 着色器程序
GLuint renderingCubeProgram;        // 着色器程序
GLuint vao[numVAOs];                // VAO 数组
GLuint vbo[numVBOs];                // VBO 数组

// 变量分配用于显示
GLuint mvLoc, projLoc;             // 模型-视图矩阵和投影矩阵的 uniform 位置
GLuint normal_matrix_loc;          // 法线矩阵的 uniform 位置
int width, height;                 // 窗口宽度和高度
float aspect;                      // 视口宽高比
glm::mat4 pMat, vMat, mMat, mvMat; // 投影矩阵、视图矩阵、模型矩阵和模型-视图矩阵

GLuint texture; // 纹理对象
GLuint texLoc;  // 纹理 uniform 位置

GLuint cubeTexture; // 立方体纹理对象
GLuint cubeLoc;     // 立方体位置 uniform 位置

Torus myTorus(0.5f, 0.2f, 48); // 圆环模型(主半径0.5，管半径0.2，48个分段)
int numTorusVertices, numTorusIndices;

// 设置顶点数据
void setupVertices(void)
{

    float cubeVertexPositions[108] =
        {-1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
         1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
         1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
         1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
         -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
         -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
         -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
         -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
         -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
         1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f};

    // 不再需要立方体纹理坐标，立方体顶点就是纹理坐标
    /*
         float cubeTextureCoord[72] =
        {
            1.00f, 0.6666666f, 1.00f, 0.3333333f, 0.75f, 0.3333333f, // back face lower right
            0.75f, 0.3333333f, 0.75f, 0.6666666f, 1.00f, 0.6666666f, // back face upper left
            0.75f, 0.3333333f, 0.50f, 0.3333333f, 0.75f, 0.6666666f, // right face lower right
            0.50f, 0.3333333f, 0.50f, 0.6666666f, 0.75f, 0.6666666f, // right face upper left
            0.50f, 0.3333333f, 0.25f, 0.3333333f, 0.50f, 0.6666666f, // front face lower right
            0.25f, 0.3333333f, 0.25f, 0.6666666f, 0.50f, 0.6666666f, // front face upper left
            0.25f, 0.3333333f, 0.00f, 0.3333333f, 0.25f, 0.6666666f, // left face lower right
            0.00f, 0.3333333f, 0.00f, 0.6666666f, 0.25f, 0.6666666f, // left face upper left
            0.25f, 0.3333333f, 0.50f, 0.3333333f, 0.50f, 0.0000000f, // bottom face upper right
            0.50f, 0.0000000f, 0.25f, 0.0000000f, 0.25f, 0.3333333f, // bottom face lower left
            0.25f, 1.0000000f, 0.50f, 1.0000000f, 0.50f, 0.6666666f, // top face upper right
            0.50f, 0.6666666f, 0.25f, 0.6666666f, 0.25f, 1.0000000f  // top face lower left
        };
*/

    numTorusVertices = myTorus.getNumVertices();
    numTorusIndices = myTorus.getNumIndices();

    std::vector<int> ind = myTorus.getIndices();
    std::vector<glm::vec3> vert = myTorus.getVertices();
    std::vector<glm::vec2> tex = myTorus.getTexCoords();
    std::vector<glm::vec3> norm = myTorus.getNormals();

    std::vector<float> pvalues;
    std::vector<float> tvalues;
    std::vector<float> nvalues;

    for (int i = 0; i < numTorusVertices; i++)
    {
        pvalues.push_back(vert[i].x);
        pvalues.push_back(vert[i].y);
        pvalues.push_back(vert[i].z);
        tvalues.push_back(tex[i].s);
        tvalues.push_back(tex[i].t);
        nvalues.push_back(norm[i].x);
        nvalues.push_back(norm[i].y);
        nvalues.push_back(norm[i].z);
    }
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(numVBOs, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexPositions), cubeVertexPositions, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0], GL_STATIC_DRAW);

    // 生成纹理对象
    texture = util.loadTexture("./texture/brick1.jpg"); // 加载纹理
    if (texture <= 0)
    {
        cout << "Error loading texture" << endl;
        // exit(1);
    } // 检查纹理加载是否成功

    glActiveTexture(GL_TEXTURE0);                                     // 激活纹理单元 0
    glBindTexture(GL_TEXTURE_2D, texture);                            // 绑定纹理对象
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 设置纹理过滤参数：最小过滤

    cubeTexture = util.loadCubeMap("./texture/cubeMap/"); // 加载纹理
    if (cubeTexture <= 0)
    {
        cout << "Error loading texture" << endl;
        // exit(1);
    } // 检查纹理加载是否成功
}

// 初始化函数
void init(GLFWwindow *window)
{
    renderingProgram = Utils::createShaderProgram("./shader/vertShader.glsl", "./shader/fragShader.glsl");
    // 创建着色器程序
    cameraX = 0.0f;
    cameraY = 0.0f;
    cameraZ = 4.0f; // 初始化摄像机位置
    cubeLocX = 0.0f;
    cubeLocY = 0.0f;
    cubeLocZ = 0.0f; // 初始化立方体位置
    setupVertices(); // 设置顶点数据

    renderingCubeProgram = Utils::createShaderProgram("./shader/vertCubeShader.glsl", "./shader/fragCubeShader.glsl");
}

// 显示函数
void display(GLFWwindow *window, double currentTime)
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // 清除深度缓冲区和颜色缓冲区

    glUseProgram(renderingCubeProgram); // 使用着色器程序

    // 获取 uniform 变量的位置
    mvLoc = glGetUniformLocation(renderingCubeProgram, "mv_matrix");
    projLoc = glGetUniformLocation(renderingCubeProgram, "proj_matrix");
    // texLoc = glGetUniformLocation(renderingProgram, "tex0"); // 获取纹理 uniform 变量的位置
    // cubeLoc = glGetUniformLocation(renderingProgram, "texSkybox"); // 获取立方体纹理 uniform 变量的位置

    // 获取窗口尺寸并计算宽高比
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 设置投影矩阵

    // 设置视图矩阵
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ)) * glm::rotate(glm::mat4(1.0f), (float)currentTime * (-0.3f), glm::vec3(0.0f, 1.0f, 0.0f));

    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(cameraX, cameraY, cameraZ));
    //* glm::rotate(glm::mat4(1.0f), (float)currentTime*(-0.3f), glm::vec3(0.0f, 1.0f, 0.0f)); // 设置模型矩阵

    mvMat = vMat * mMat; // 计算模型-视图矩阵

    // draw skybox

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); // 绑定 VBO

    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (void *)0); // 设置顶点属性指针：坐标
    glEnableVertexAttribArray(0);                               // 启用顶点属性

    glActiveTexture(GL_TEXTURE0);                    // 激活纹理单元 1
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture); // 绑定纹理对象

    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

    glDisable(GL_DEPTH_TEST);          // 关闭深度测试
    glEnable(GL_CULL_FACE);            // 开启面剔除
    glFrontFace(GL_CCW);               // 设置正面为顺时针
    glDrawArrays(GL_TRIANGLES, 0, 36); // 绘制三角形

    glUseProgram(renderingProgram); // 使用着色器程序

    // 获取 uniform 变量的位置
    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
    normal_matrix_loc = glGetUniformLocation(renderingProgram, "normal_matrix");
    // draw torus

    // 设置模型矩阵
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(cubeLocX, cubeLocY, cubeLocZ)) * glm::rotate(glm::mat4(1.0f), (float)currentTime * (-0.3f), glm::vec3(1.0f, 1.0f, 0.0f));

    mvMat = vMat * mMat; // 计算模型-视图矩阵

    // 将矩阵传递到着色器
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

    glm::mat4 normal_matrix = glm::transpose(glm::inverse(mvMat));                     // 计算法线矩阵
    glUniformMatrix4fv(normal_matrix_loc, 1, GL_FALSE, glm::value_ptr(normal_matrix)); // 将法线矩阵传递到着色器

    glEnable(GL_DEPTH_TEST); // 启用深度测试
    glDepthFunc(GL_LEQUAL);  // 设置深度测试函数
    glFrontFace(GL_CW);      // 设置正面为逆时针

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);                      // 绑定 VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (void *)0); // 设置顶点属性指针：坐标
    glEnableVertexAttribArray(0);                               // 启用顶点属性

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]); // 绑定 VBO
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, (void *)0);
    glEnableVertexAttribArray(1); // 启用顶点属性

    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);                      // 绑定 VBO
    glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, (void *)0); // 设置顶点属性指针：坐标
    glEnableVertexAttribArray(2);                               // 启用顶点属性

    glActiveTexture(GL_TEXTURE0); // 激活纹理单元 0
    // glBindTexture(GL_TEXTURE_2D, texture);                            // 绑定纹理对象
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 设置纹理过滤参数：最小过滤
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture); // 绑定纹理对象

    glDrawElements(GL_TRIANGLES, numTorusIndices, GL_UNSIGNED_INT, 0); // 绘制圆环
}

// 主函数
int main(void)
{
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    } // 初始化 GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);                              // 设置 OpenGL 主版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                              // 设置 OpenGL 次版本号
    GLFWwindow *window = glfwCreateWindow(1200, 1200, "Chapter 5", NULL, NULL); // 创建窗口
    glfwMakeContextCurrent(window);                                             // 设置当前上下文
    if (glewInit() != GLEW_OK)
    {
        exit(EXIT_FAILURE);
    } // 初始化 GLEW
    glfwSwapInterval(1); // 开启垂直同步

    init(window); // 初始化

    // 主循环
    while (!glfwWindowShouldClose(window))
    {
        display(window, glfwGetTime()); // 调用显示函数
        glfwSwapBuffers(window);        // 交换缓冲区
        glfwPollEvents();               // 处理事件
    }

    glfwDestroyWindow(window); // 销毁窗口
    glfwTerminate();           // 终止 GLFW
    exit(EXIT_SUCCESS);        // 正常退出
}