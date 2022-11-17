#pragma once

#include <cstring>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Kanji {
    // window class contains all the window related things
    class Window {

        private:
            GLFWwindow* glfwWindow;
            int width = 640;
            int height = 480;
            bool fullScreen = false;
            char title [32] = "Kanji App";

        public:
            //window size 
            void setSize(const int _width,const int _height);
            int getWidth();
            int getHeight();

            //window title
            void setTitle(const char* _title);

            //window get glfwWinodw
            GLFWwindow* getGLFWwindow();

            //window creation/destroy
            void create();
            void destroy();

            //window should close
            bool isClosed();

    };

}