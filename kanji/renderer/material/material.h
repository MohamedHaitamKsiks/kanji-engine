#pragma once


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "renderer/pipeline.h"

#define MATERIAL_DEFAULT 0

namespace Kanji {

    typedef unsigned int Material;

    struct MaterialInfo {
        Pipeline pipeline;
    };

}