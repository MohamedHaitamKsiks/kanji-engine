#pragma once

#include "core/math/math.h"

namespace Kanji {

    typedef unsigned int Mesh;

    typedef struct MeshInstance {
        Mesh mesh;
        // init transform
        mat4 transform;
    } MeshInstance;

}