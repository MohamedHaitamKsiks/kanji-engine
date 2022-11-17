#include "window.h"

namespace Kanji {

    //window size 
    //window set size
    void Window::setSize(const int _width,const int _height) {
        width = _width;
        height = _height;
        glfwSetWindowSize(glfwWindow, width, height);
    }
    //window get width
    int Window::getWidth() {
        return width;
    }
    // window get height
    int Window::getHeight() {
        return height;
    }

    //window set title
    void Window::setTitle(const char* _title) {
        strcpy(title, _title);
        glfwSetWindowTitle(glfwWindow, title);
    }

    //get glfw window
    GLFWwindow* Window::getGLFWwindow() {
        return glfwWindow;
    }

    //window init
    void Window::create() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        //create the window
        glfwWindow = glfwCreateWindow(width, height,title, nullptr, nullptr);
    }

    //is window closed
    bool Window::isClosed() {
        return glfwWindowShouldClose(glfwWindow);
    }

}