#pragma once


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Kanji {

    typedef unsigned int Texture;

    typedef struct TextureInfo {
        VkImage image;
        VkDeviceMemory imageMemory;
        VkImageView imageView;
        uint32_t width;
        uint32_t height;
        VkFormat format;
        VkSampler sampler;
    } TextureInfo;

}

