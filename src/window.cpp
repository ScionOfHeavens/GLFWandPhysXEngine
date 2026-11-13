#pragma once

#include <iostream>
#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "shader_program_builder.hpp"
#include "camera.hpp"
#include "window.hpp"

void Window::init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Window::setWindowSize(unsigned int screenWidth, unsigned int screenHeight){
    mScreenWidth = screenWidth;
    mScreenHeight = screenHeight;
}

void Window::setWindowTitle(const char* title){
    mTitle = title;
}

void Window::setRenderFunc(std::function<void(Camera, float, float)> renderStepFunc) { mRenderStepfunc = renderStepFunc; }

void Window::buildContext() {
    mWindow= glfwCreateWindow(mScreenWidth, mScreenHeight, mTitle, NULL, NULL);

    glfwMakeContextCurrent(mWindow);
    glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
    glfwSetCursorPosCallback(mWindow, mouse_callback);
    glfwSetScrollCallback(mWindow, scroll_callback); 
    
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glEnable(GL_DEPTH_TEST);
}

void Window::run() {
    while (!glfwWindowShouldClose(mWindow))
    {
        float currentFrame = glfwGetTime();
        mDeltaTime = currentFrame - mLastFrame;
        mLastFrame = currentFrame;
        
        processInput(mWindow);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        mRenderStepfunc(mCamera, (float)mScreenWidth, (float)mScreenHeight);

        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
}

unsigned int Window::mScreenWidth = 600;
unsigned int Window::mScreenHeight = 400;
const char* Window::mTitle = "Window";
Camera Window::mCamera = Camera(glm::vec3(-0.6f, 5.83f, 19.5f));;
float Window::mLastX =  Window::mScreenWidth / 2.0f;
float Window::mLastY = Window::mScreenHeight / 2.0f;

bool Window::mFirstMouse = true;

float Window::mDeltaTime = 0.0f;
float Window::mLastFrame = 0.0f;

GLFWwindow* Window::mWindow;

std::function<void(Camera, float, float)> Window::mRenderStepfunc = [](Camera, float, float){};

void Window::processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mCamera.ProcessKeyboard(FORWARD, mDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mCamera.ProcessKeyboard(BACKWARD, mDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mCamera.ProcessKeyboard(LEFT, mDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mCamera.ProcessKeyboard(RIGHT, mDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        mCamera.ProcessKeyboard(UP, mDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        mCamera.ProcessKeyboard(DOWN, mDeltaTime);
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Window::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (mFirstMouse)
    {
        mLastX = (float)xpos;
        mLastY = (float)ypos;
        mFirstMouse = false;
    }

    float xoffset = (float)xpos - mLastX;
    float yoffset = mLastY - (float)ypos;

    mLastX = (float)xpos;
    mLastY = (float)ypos;

    mCamera.ProcessMouseMovement(xoffset, yoffset);
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    mCamera.ProcessMouseScroll((float)yoffset);
}