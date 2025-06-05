#pragma once

#include <GLFW/glfw3.h>

#include <iostream>
using namespace std;

#include "colors.h"

struct Screen
{
    GLFWwindow* handle;
    int width;
    int height;
};

inline Screen screen;

inline void init(bool borderless = false)
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return;
    }

    if (borderless) glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
}
#define INIT1 init()
#define INIT2 init(true)

#define SCREEN_WIDTH screen.width
#define SCREEN_HEIGHT screen.height

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

inline void window(string title, int width, int height)
{
    GLFWwindow* win = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    
    if (!win)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
    }

    screen = { win, width, height };
    glfwMakeContextCurrent(screen.handle);
    
    glfwSetFramebufferSizeCallback(screen.handle, framebuffer_size_callback);
    glViewport(0, 0, screen.width, screen.height);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    screen.width = width;
    screen.height = height;
}

inline bool window_is_open() { return !glfwWindowShouldClose(screen.handle); }

#define RUNNING window_is_open() 
#define EVENTS glfwPollEvents()

inline void clear(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

inline void clear(const char* hex)
{
    Color color = to_color(hex);
    clear(color.r, color.g, color.b, color.a);
}
#define CLEAR_BUFFER clear(bgcolor.r, bgcolor.g, bgcolor.b, bgcolor.a)

inline void flip() { glfwSwapBuffers(screen.handle); }
#define FLIP_BUFFER flip()

inline int exit()
{
    glfwDestroyWindow(screen.handle);
    glfwTerminate();

    return 0;
}
#define EXIT exit()