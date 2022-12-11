#pragma once

#include <stdexcept>
#include <string>
#include "mesh/mesh.h"
#include "vcontext.h"
#include "image/image.h"
#include "image/texture.h"
#include "material/material.h"
#include "core/memory/allocator.h"

namespace Kanji {

    class Renderer {
        public:
            // init/delete
            void init(VContext* _vcontext);
            void destroy();

            //material
            Material materialCreate(const std::string &vert,const std::string &frag);
            void materialFree(Material material);

            // mesh
            // load mesh from list of verticies and indices
            Mesh meshLoad(std::vector<Vertex>& vertices, std::vector<uint16_t>& indices);
            // free mesh from memory
            void meshFree(Mesh mesh);
            // set / get mesh material
            void meshSetMaterial(Mesh mesh, Material material);
            Material meshGetMaterial(Mesh mesh);
            // create instance
            MeshInstance* meshInstanceCreate(Mesh mesh);
            // draw frame
            void drawFrame();

            //texture
            Texture textureImageLoad(Image image);
            void textureFree(Texture texture);


        private:
            //vcontext
            VContext* vcontext;
            // loaded meshs
            PoolAllocator<MeshInfo> meshInfos;
            // mesh instances
            PoolAllocator<MeshInstance> meshInstances;
            // textures
            PoolAllocator<TextureInfo> textureInfos;
            //materials
            PoolAllocator<MaterialInfo> materialInfos;

    };

}