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
#include "mesh/mesh.h"
#include "window.h"
#include "buffer.h"
#include "vdevice.h"

#define VERTEX_BUFFER_SIZE UINT16_MAX
#define INDEX_BUFFER_SIZE UINT16_MAX

// vulkan cotnext to use trhe vulkan api
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


    //push constant struct
    struct PushConstant {
        mat4 transform;
    };


    class VContext {

        public:
            //init vulkan context
            void init(Window* _window);
            //destroy vulkan context
            void destroy();

            //buffers
            //vertex buffer
            Buffer vertexBuffer;
            //index buffer
            Buffer indexBuffer;

            // draw frame funcitons
            // draw frame start 
            void drawFrameStart(uint32_t* imageIndex);
            // draw frame end
            void drawFrameEnd(uint32_t* imageIndex);
           
            // command buffer start / end record
            void commandBufferStartRecord(uint32_t imageIndex);
            void commandBufferEndRecord();
            
            //mesh
            void meshDraw(MeshInfo meshInfo, PushConstant* pushConstant);
            void meshBind(MeshInfo meshInfo);

            
        private:
            // window
            Window* window;
            // vulkan
            // vulkan instance
            VkInstance instance;
            void vulkanInstanceCreate();
            // vulkan surface
            VkSurfaceKHR surface;
            void vulkanSurfaceCreate();
            // vulkan device
            VDevice vdevice;
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

            // vulkan sync objects
            struct {
                VkSemaphore imageAvailableSemaphore;
                VkSemaphore renderFinishedSemaphore;
                VkFence inFlightFence;
            } syncObjects;

            // sync objects methods
            void syncObjectsCreate();
            void syncObjectsDestroy();



    };

}