#pragma once

#include<vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Kanji {

    // pipeline
    struct Pipeline {
        VkPipelineLayout layout;
        VkPipeline graphics;
        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
    };

}