#pragma once

#include<cstdint>

namespace Kanji {

    typedef struct MeshInfo {
        // vertex data
        uint16_t vertexBufferIndex;
        uint16_t vertexBufferSize;
        // index data
        uint16_t indexBufferIndex;
        uint16_t indexBufferSize;
    } MeshInfo;

}