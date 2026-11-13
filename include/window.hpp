#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "camera.hpp"

#include <iostream>
#include <functional>

class Window{
public:
    static void init();
    static void setWindowSize(unsigned int screenWidth, unsigned int screenHeight);
    static void setWindowTitle(const char* title);
    static void setRenderFunc(std::function<void(Camera, float, float)> renderStepFunc);
    static void buildContext();
    static void run();

private:
    static unsigned int mScreenWidth;
    static unsigned int mScreenHeight;
    static const char* mTitle;
    static Camera mCamera;
    static float mLastX;
    static float mLastY;

    static bool mFirstMouse;

    static float mDeltaTime;
    static float mLastFrame;

    static GLFWwindow* mWindow;

    static std::function<void(Camera, float, float)>mRenderStepfunc;

    static void processInput(GLFWwindow* window);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};
