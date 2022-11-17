#include "renderer.h"

namespace Kanji {

    // init renderer
    void Renderer::init(VContext* _vcontext) {
        vcontext = _vcontext;
        // create mesh & meshinstance allocators
        meshInfos.init(1024);
        meshInstances.init(1024);
    }

    // load mesh from list of verticies and indices
    Mesh Renderer::meshLoad(std::vector<Vertex>& vertices, std::vector<uint16_t>& indices) {
        // get mesh data
        MeshInfo meshInfo;
        meshInfo.vertexBufferIndex = vcontext->vertexBuffer.head;
        meshInfo.indexBufferIndex = vcontext->indexBuffer.head;
        meshInfo.vertexBufferSize = vertices.size() * sizeof(Vertex);
        meshInfo.indexBufferSize = indices.size() * sizeof(uint16_t);

        // push data to the buffers
        vcontext->vertexBuffer.push(vertices.data(), meshInfo.vertexBufferSize);
        vcontext->indexBuffer.push(indices.data(), meshInfo.indexBufferSize);
   
        // push data to the mesh allocator
        Mesh mesh = meshInfos.alloc();
        *(meshInfos.get(mesh)) = meshInfo;

        // return mesh
        return mesh;
    }

    // free mesh from memory
    void Renderer::meshFree(Mesh mesh) {
        //get mesh info
        MeshInfo meshInfo = *(meshInfos.get(mesh));
        //free mesh from buffers
        vcontext->vertexBuffer.remove(meshInfo.vertexBufferIndex, meshInfo.vertexBufferSize);
        vcontext->indexBuffer.remove(meshInfo.indexBufferIndex, meshInfo.indexBufferSize);
        //free mesh info from allocator
        meshInfos.free(mesh);
    }

    //mesh create instance
    MeshInstance* Renderer::meshInstanceCreate(Mesh mesh) {
        //allocate new mesh instance
        ChunkId meshInstanceIndex = meshInstances.alloc();
        //push newMeshInstance values to the alloc
        MeshInstance* meshInstance = meshInstances.get(meshInstanceIndex);
        meshInstance->mesh = mesh;
        meshInstance->transform = mat4::identity();
        //return mesh isntance
        return meshInstance;
    }

    // draw frame
    void Renderer::drawFrame() {
        //current image index
        uint32_t imageIndex;
        //start drawting the frame
        vcontext->drawFrameStart(&imageIndex);

        //drawing elements to the screen
        //...

        //draw all mesh Instances
        for (ChunkId i = 0; i < meshInstances.getMaxSize(); i++) {
            if (meshInstances.isUsed(i)) {
                MeshInstance* instance = meshInstances.get(i);
                MeshInfo meshInfo = *meshInfos.get(instance->mesh);
                PushConstant pushConstant = PushConstant{
                    instance->transform
                };
                vcontext->meshBind(meshInfo);
                vcontext->meshDraw(meshInfo, &pushConstant);
            }
        }

        //end drawing the frame
        vcontext->drawFrameEnd(&imageIndex);
    }

}