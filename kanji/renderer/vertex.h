#pragma once

#include <array>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "core/math/math.h"


namespace Kanji {
    // vertex definition 
    typedef struct Vertex {
        vec3 position;
        vec3 color;
        // binding description
        static VkVertexInputBindingDescription getBindingDescription();
        //attribute descriptions
        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
    } Vertex;

}