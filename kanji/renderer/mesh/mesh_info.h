#pragma once

#include<cstdint>
#include "renderer/material/material.h"

namespace Kanji {

    typedef struct MeshInfo {
        // vertex data
        uint16_t vertexBufferIndex;
        uint16_t vertexBufferSize;
        // index data
        uint16_t indexBufferIndex;
        uint16_t indexBufferSize;
        // Material
        Material material;
    } MeshInfo;

}