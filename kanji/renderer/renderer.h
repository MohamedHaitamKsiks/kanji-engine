#pragma once

#include <stdexcept>
#include "mesh/mesh.h"
#include "vcontext.h"
#include "core/memory/allocator.h"

namespace Kanji {

    class Renderer {
        public:
            // init/delete
            void init(VContext* _vcontext);
            void destroy();
            // mesh
            // load mesh from list of verticies and indices
            Mesh meshLoad(std::vector<Vertex>& vertices, std::vector<uint16_t>& indices);
            // free mesh from memory
            void meshFree(Mesh mesh);
            // create instance
            MeshInstance* meshInstanceCreate(Mesh mesh);
            // draw frame
            void drawFrame();

        private:
            //vcontext
            VContext* vcontext;
            // loaded meshs
            PoolAllocator<MeshInfo> meshInfos;
            // mesh instances
            PoolAllocator<MeshInstance> meshInstances;
    };

}