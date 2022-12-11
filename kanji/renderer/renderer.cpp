#include "renderer.h"

namespace Kanji {

    // init renderer
    void Renderer::init(VContext* _vcontext) {
        vcontext = _vcontext;
        // create mesh & meshinstance allocators
        meshInfos.init(1024);
        meshInstances.init(1024);
        materialInfos.init(1024);
        textureInfos.init(1024);
        //create default material
        materialCreate(
            "shaders/main.vert.spv", 
            "shaders/main.frag.spv"
        );
    }

    //material
    //material create
    Material Renderer::materialCreate(const std::string &vert,const std::string &frag) {
        //create material info
        MaterialInfo materialInfo;
        materialInfo.pipeline = vcontext->pipelineCreate(vert, frag);
        //allocate material
        Material material = materialInfos.alloc();
        //set material
        (*materialInfos.get(material)) = materialInfo;
        return material;
    }
    // material free
    void Renderer::materialFree(Material material) {
        materialInfos.free(material);
    }

    // load mesh from list of verticies and indices
    Mesh Renderer::meshLoad(std::vector<Vertex>& vertices, std::vector<uint16_t>& indices) {
        // get mesh data
        MeshInfo meshInfo;
        meshInfo.vertexBufferIndex = vcontext->vertexBuffer.head;
        meshInfo.indexBufferIndex = vcontext->indexBuffer.head;
        meshInfo.vertexBufferSize = vertices.size() * sizeof(Vertex);
        meshInfo.indexBufferSize = indices.size() * sizeof(uint16_t);
        meshInfo.material = MATERIAL_DEFAULT;
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

    //mesh material set / get
    void Renderer::meshSetMaterial(Mesh mesh, Material material) {
        MeshInfo* meshInfo = meshInfos.get(mesh);
        meshInfo->material = material;
    }
    
    Material Renderer::meshGetMaterial(Mesh mesh) {
        MeshInfo* meshInfo = meshInfos.get(mesh);
        return meshInfo->material;
    }

    //textures
    //texture load from image
    Texture Renderer::textureImageLoad(Image image) {
        //copy iamge data to image buffer
        vcontext->imageBuffer.push(image.data, 4 * image.width * image.height);
        //texture info
        TextureInfo textureInfo;
        textureInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        //image create
        vcontext->imageCreate(&textureInfo);
        //image layout transition to transfer from buffer to image
        vcontext->imageLayoutTransition(&textureInfo, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        //image buffer
        vcontext->imageCopyBufferToImage(&textureInfo);
        //image layout transition to shader read only
        vcontext->imageLayoutTransition(&textureInfo, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        // image buffer clear
        vcontext->imageBuffer.clear();
        // image view craete
        textureInfo.imageView = vcontext->imageViewCreate(textureInfo.image, textureInfo.format);
        // image sampler create
        vcontext->imageSamplerCreate(&textureInfo);
        //allocate texture
        Texture texture = textureInfos.alloc();
        *textureInfos.get(texture) = textureInfo;
        //return texture
        return texture;
    }

    //texture free
    void Renderer::textureFree(Texture texture) {
        TextureInfo* textureInfo = textureInfos.get(texture);
        vcontext->imageSamplerDestroy(textureInfo);
        vcontext->imageViewDestroy(textureInfo->imageView);
        vcontext->imageDestroy(textureInfo);
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
                MaterialInfo materialInfo = *materialInfos.get(meshInfo.material);

                PushConstant pushConstant = PushConstant{
                    instance->transform
                };
                
                vcontext->pipelineBind(materialInfo.pipeline);
                vcontext->meshBind(meshInfo);
                vcontext->meshDraw(meshInfo, &pushConstant, materialInfo.pipeline);
            }
        }

        //end drawing the frame
        vcontext->drawFrameEnd(&imageIndex);
    }

}