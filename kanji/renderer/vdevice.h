#pragma once

#include<vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Kanji {

    typedef struct VDevice {
        VkDevice device;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        VkPhysicalDevice physicalDevice;
        const std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        float queuePriority = 1.0f;
    } VDevice;

}

