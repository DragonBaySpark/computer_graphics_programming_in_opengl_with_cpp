/*
* 第2章 OpenGL图形管线示例程序
* 这个程序演示了OpenGL的基本设置和渲染循环
* 创建一个600x600像素的窗口并将其背景设置为红色
*/

// GLEW是OpenGL的扩展加载库
#include <GL\glew.h>
// GLFW提供了创建窗口和处理用户输入的功能
#include <GLFW\glfw3.h>
// 用于输出错误信息
#include <iostream>

using namespace std;

/**
 * 初始化OpenGL的设置
 * @param window 目标窗口的指针
 */
void init(GLFWwindow* window) { }

/**
 * 渲染函数，负责实际的绘制操作
 * @param window 目标窗口的指针
 * @param currentTime 当前时间，可用于动画
 */
void display(GLFWwindow* window, double currentTime) {
    // 设置清除缓冲区时要使用的颜色(红色)
    glClearColor(1.0, 0.0, 0.0, 1.0);
    // 使用上面设置的颜色清除颜色缓冲区
    glClear(GL_COLOR_BUFFER_BIT);
}

/**
 * 主函数：程序入口点
 * 负责初始化GLFW和GLEW，创建窗口，并进入渲染循环
 */
int main(void) {
    // 初始化GLFW库，如果失败则退出
    if (!glfwInit()) { exit(EXIT_FAILURE); }
    
    // 设置OpenGL版本为4.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    
    // 创建一个600x600像素的窗口
    GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter 2 - program 1", NULL, NULL);
    
    // 将新创建的窗口设置为当前OpenGL上下文
    glfwMakeContextCurrent(window);
    
    // 初始化GLEW库，用于管理OpenGL扩展，如果失败则退出
    if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
    
    // 启用垂直同步，防止画面撕裂
    glfwSwapInterval(1);

    // 调用自定义的初始化函数
    init(window);

    // 渲染循环：直到窗口被关闭才退出
    while (!glfwWindowShouldClose(window)) {
        // 调用显示函数进行渲染，传入当前时间用于动画
        display(window, glfwGetTime());
        // 交换前后缓冲区，将渲染结果显示到屏幕上
        glfwSwapBuffers(window);
        // 处理窗口事件（如键盘输入、鼠标移动等）
        glfwPollEvents();
    }

    // 清理资源并退出
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}