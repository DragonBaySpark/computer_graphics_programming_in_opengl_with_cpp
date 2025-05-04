/*
* 第2章 OpenGL绘制点示例程序
* 这个程序演示了如何使用OpenGL的着色器程序绘制一个点
* 包含了着色器程序的加载、编译和链接过程
*/

// OpenGL相关库
#include <GL\glew.h>      // GLEW用于加载OpenGL函数
#include <GLFW\glfw3.h>   // GLFW用于创建窗口和处理输入

// 标准库
#include <iostream>        // 用于控制台输出
#include <string>         // 用于字符串处理
#include <fstream>        // 用于文件读取
#include <filesystem>     // 用于文件路径处理

using namespace std;

// 定义顶点数组对象（VAO）的数量
#define numVAOs 1

// 全局变量
GLuint renderingProgram;  // 存储编译后的着色器程序
GLuint vao[numVAOs];     // 存储顶点数组对象


/**
 * 打印着色器编译日志
 * 用于调试着色器编译错误
 * @param shader 着色器对象的ID
 */
void printShaderLog(GLuint shader) {
    int len = 0;          // 日志长度
    int chWrittn = 0;     // 实际写入的字符数
    char *log;            // 日志内容
    // 获取日志长度
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        log = (char *)malloc(len);
        // 获取实际的日志信息
        glGetShaderInfoLog(shader, len, &chWrittn, log);
        cout << "着色器编译日志: " << log << endl;
        free(log);
    }
}

/**
 * 打印着色器程序链接日志
 * 用于调试着色器程序链接错误
 * @param prog 着色器程序的ID
 */
void printProgramLog(int prog) {
    int len = 0;          // 日志长度
    int chWrittn = 0;     // 实际写入的字符数
    char *log;            // 日志内容
    // 获取日志长度
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        log = (char *)malloc(len);
        // 获取实际的日志信息
        glGetProgramInfoLog(prog, len, &chWrittn, log);
        cout << "程序链接日志: " << log << endl;
        free(log);
    }
}

/**
 * 检查OpenGL错误
 * 遍历所有待处理的错误并打印
 * @return 如果发现错误返回true，否则返回false
 */
bool checkOpenGLError() {
    bool foundError = false;
    int glErr = glGetError();
    while (glErr != GL_NO_ERROR) {
        cout << "OpenGL错误代码: " << glErr << endl;
        foundError = true;
        glErr = glGetError();
    }
    return foundError;
}




/**
 * 从文件中读取着色器源代码
 * @param filePath 着色器文件的路径
 * @return 包含着色器源代码的字符串
 * 
 * 这个函数负责：
 * 1. 打开并读取着色器源文件
 * 2. 将文件内容转换为字符串
 * 3. 处理文件打开失败的情况
 */
string readFile(const char *filePath) {
    string content;
    ifstream fileStream(filePath, ios::in);
    string line = "";
    
    // 检查文件是否成功打开
    if (!fileStream.is_open()) {
        cout << "无法打开文件: " << filePath << endl;
        return content;
    }
    // 逐行读取文件内容
    while (getline(fileStream, line)) {
        content.append(line + "\n");
    }
    fileStream.close();
    return content;
}
/**
 * 创建、编译和链接着色器程序
 * @return 返回编译和链接成功的着色器程序ID
 * 
 * 这个函数完成以下步骤：
 * 1. 创建着色器对象
 * 2. 读取并编译顶点着色器和片段着色器
 * 3. 创建着色器程序并链接着色器
 * 4. 进行错误检查和状态报告
 */
GLuint createShaderProgram() {
    // 编译状态变量
    GLint vertCompiled;    // 顶点着色器编译状态
    GLint fragCompiled;    // 片段着色器编译状态
    GLint linked;          // 程序链接状态

    // 创建着色器和程序对象
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);      // 创建顶点着色器
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);    // 创建片段着色器
    GLuint vfprogram = glCreateProgram();                   // 创建着色器程序

    // 读取着色器源代码
    string vertShaderStr = readFile("./shader/vertShader.glsl");
    string fragShaderStr = readFile("./shader/fragShader.glsl");
    const char *vertShaderSrc = vertShaderStr.c_str();
    const char *fragShaderSrc = fragShaderStr.c_str();

    // 将源代码加载到着色器对象中
    glShaderSource(vShader, 1, &vertShaderSrc, NULL);
    glShaderSource(fShader, 1, &fragShaderSrc, NULL);

    // 编译顶点着色器
    glCompileShader(vShader);
    checkOpenGLError();    // 检查OpenGL错误
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
    if (vertCompiled == 1) {
         cout << "顶点着色器编译成功" << endl;
    }
    else {
        cout << "顶点着色器编译失败" << endl;
        printShaderLog(vShader);
    }

    // 编译片段着色器
    glCompileShader(fShader);
    checkOpenGLError();    // 检查OpenGL错误
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
    if (fragCompiled == 1) {
        cout << "片段着色器编译成功" << endl;
    }
    else {
        cout << "片段着色器编译失败" << endl;
        printShaderLog(fShader);
    }

    // 将着色器附加到程序对象并链接
    glAttachShader(vfprogram, vShader);    // 附加顶点着色器
    glAttachShader(vfprogram, fShader);    // 附加片段着色器
    glLinkProgram(vfprogram);              // 链接程序

    // 检查链接状态
    checkOpenGLError();    // 检查OpenGL错误
    glGetProgramiv(vfprogram, GL_LINK_STATUS, &linked);
    if (linked == 1) {
        cout << "着色器程序链接成功" << endl;
    }
    else {
        cout << "着色器程序链接失败" << endl;
        printProgramLog(vfprogram);
    }
    cout << "着色器程序创建完成，ID: " << vfprogram << endl;
    return vfprogram;
}


/**
 * 初始化OpenGL的设置
 * @param window 目标窗口的指针
 */
void init(GLFWwindow* window) { 
	
	renderingProgram = createShaderProgram();
	glGenVertexArrays(numVAOs, vao);
	glBindVertexArray(vao[0]);
	
}

/**
 * 渲染函数，负责实际的绘制操作
 * @param window 目标窗口的指针
 * @param currentTime 当前时间，可用于动画
 */
// 该函数用于渲染图形，接受两个参数：
// window：GLFW窗口对象的指针
// currentTime：当前时间，可用于实现动画效果
void display(GLFWwindow *window, double currentTime)
{
    // 清除颜色缓冲区和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static float offset=0.0f; // 偏移量，用于动画效果
    static float delta=0.01f; // 偏移增量
    offset+=delta; // 更新偏移量

    if(offset>0.5f) delta=-0.01f;
    if(offset<-0.5f) delta=0.01f; // 如果偏移量超过范围，反转增量

	// 使用名为 renderingProgram 的着色器程序
	glUseProgram(renderingProgram);
	// 设置要绘制的点的大小为 1 像素
	glPointSize(1.0f);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // 设置绘制模式为填充
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// 绘制点图元，从数组的第 0 个元素开始，绘制 3 个点
    GLuint offsetLocation = glGetUniformLocation(renderingProgram, "offsetX"); // 获取偏移量uniform变量的位置
    glUniform1f(offsetLocation, offset); // 设置偏移量uniform变量的值
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

/**
 * 主函数：程序入口点
 * 负责初始化GLFW和GLEW，创建窗口，并进入渲染循环
 */
int main(void) {
	// 获取当前工作目录的路径
	std::filesystem::path currentPath = std::filesystem::current_path();

	// 获取绝对路径
	std::filesystem::path absolutePath = std::filesystem::absolute(currentPath);

	// 提取目录名称
	std::string directoryName = currentPath.filename().string();

	std::cout << "absolute_path: " << absolutePath.string() << std::endl;

    system("chcp 65001 > nul"); // 设置为 UTF-8 编码
	
    // 初始化GLFW库，如果失败则退出
    if (!glfwInit()) { exit(EXIT_FAILURE); }
    
    // 设置OpenGL版本为4.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    
    // 创建一个600x600像素的窗口
    GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter 2 - program 3", NULL, NULL);
    
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