#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <optional>
#include <set>
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <cstring>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "core/math/math.h"
#include "core/memory/allocator.h"
#include "core/time/time.h"
#include "vertex.h"
#include "mesh.h"


#define VERTEX_BUFFER_SIZE UINT16_MAX
#define INDEX_BUFFER_SIZE UINT16_MAX

namespace Kanji {
    
    // queue family indicies
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
    };

    // swap chain support details
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    // buffers
    typedef struct Buffer {
        VkBuffer buffer;
        VkDeviceSize size;
        VkDeviceMemory memory;
        uint16_t head = 0;
    } Buffer;

    //push constant struct
    typedef struct PushConstant {
        mat4 transform;
    } PushConstant;

    //vulkan app class
    class VApp {

        public:
            // window
            // window size
            void windowSetSize(vec2 size);
            vec2 windowGetSize();
            // window fullscreen
            void windowSetFullScreen(bool fullScreen);
            bool windowGetFullScreen();

            // mesh
            // load mesh from list of verticies and indices
            Mesh meshLoad(std::vector<Vertex> vertices, std::vector<uint16_t> indices);
            // free mesh from memory
            void meshFree(Mesh mesh);
            // create instance
            MeshInstance* meshInstanceCreate(Mesh mesh);
            
            //init vulkan app
            void init();
            void start(void (*update)(double delta));
            void destroy();


        private:
            // window
            struct {
                GLFWwindow* glfwWindow;
                int width = 640;
                int height = 480;
                bool fullScreen = false;
                char title [32] = "Kanji App";
            } window;
            void windowInit();

            // vulkna
            // vulkan instance
            VkInstance instance;
            void vulkanInstanceCreate();
            // vulkan surface
            VkSurfaceKHR surface;
            void vulkanSurfaceCreate();
            // vulkan device
            struct {
                VkDevice device;
                VkQueue graphicsQueue;
                VkQueue presentQueue;
                VkPhysicalDevice physicalDevice;
                const std::vector<const char*> deviceExtensions = {
                    VK_KHR_SWAPCHAIN_EXTENSION_NAME
                };
                float queuePriority = 1.0f;
            } vdevice;
            // vulkan device methods
            void deviceCreate();
            void devicePickPhysicalDevice();
            bool deviceIsDeviceSuitable(VkPhysicalDevice devidce, int currentDeviceIndex);
            bool deviceCheckDeviceExtensionSupport(VkPhysicalDevice devide);
            QueueFamilyIndices deviceFindQueueFamilies(VkPhysicalDevice device);
            SwapChainSupportDetails deviceQuerySwapChainSupport(VkPhysicalDevice device);
            void deviceCreateLogicalDevice();

            // vulkan swap chain
            struct {
                VkSwapchainKHR swapChain;
                std::vector<VkImage> swapChainImages;
                VkFormat swapChainImageFormat;
                VkExtent2D swapChainExtent;    
            } vswapChain;
            // vulkan swap chain methods
            VkSurfaceFormatKHR swapChainChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
            VkPresentModeKHR swapChainChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
            VkExtent2D swapChainChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
            void swapChainCreate();
            
            // vulkan images view
            std::vector<VkImageView> imageViews;
            void imageViewsCreate();
            void imageViewDestroy();

            // vulkan pipeline
            struct {
                VkPipelineLayout layout;
                VkPipeline graphics;
                std::vector<VkDynamicState> dynamicStates = {
                    VK_DYNAMIC_STATE_VIEWPORT,
                    VK_DYNAMIC_STATE_SCISSOR
                };
            } vpipeline;
            std::vector<char> readFile(const std::string& filePath);
            VkShaderModule pipelineCreateShaderModule(const std::vector<char>& code);
            void pipelineCreate(const std::string& vertFilePath, const std::string& fragFilePath);
            void pipelineDestroy();

            // buffer methods
            uint32_t bufferFindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
            void bufferCreate(Buffer* buffer, size_t bufferSize);
            void bufferDestroy(Buffer* buffer);
            void bufferPush(Buffer* buffer, const void* bufferData, const size_t size);
            void bufferDelete(Buffer* buffer, const uint16_t index, const size_t size);

            //vertex buffer
            Buffer vertexBuffer;
            //index buffer
            Buffer indexBuffer;

            // vulkan render pass
            VkRenderPass renderPass;
            void renderPassCreate();

            // vulkan buffers
            // frame buffers
            std::vector<VkFramebuffer> frameBuffers;
            void frameBuffersCreate();
            void frameBufferDestroy();
            // command pool
            VkCommandPool commandPool;
            void commandPoolCreate();
            void commandPoolDestroy();
            // command buffer
            VkCommandBuffer commandBuffer;
            void commandBufferCreate();
            void commandBufferRecord(uint32_t imageIndex);

            // vulkan sync objects
            struct {
                VkSemaphore imageAvailableSemaphore;
                VkSemaphore renderFinishedSemaphore;
                VkFence inFlightFence;
            } syncObjects;
            // sync objects methods
            void syncObjectsCreate();
            void syncObjectsDestroy();

            //after setting up vulkan for the vulkan app we can finally start rendering somthing in the screen
            // draw frame
            void drawFrame();

            // draw mesh
            void meshDraw(MeshInfo meshInfo, PushConstant* pushConstant);
            void meshBind(MeshInfo meshInfo);

            // loaded meshs
            PoolAllocator<MeshInfo> meshInfos;
            // mesh instances
            PoolAllocator<MeshInstance> meshInstances;


    };

}