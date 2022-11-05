#include "vapp.h"

namespace Kanji {

    // window
    // window init
    void VApp::windowInit() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        //create the window
        window.glfwWindow = glfwCreateWindow(window.width, window.height, window.title, nullptr, nullptr);

    }


    // window size
    void VApp::windowSetSize(vec2 size) {
        window.width = static_cast<int>(size.x);
        window.height = static_cast<int>(size.y);
        // resize glfw window
        glfwSetWindowSize(window.glfwWindow, window.width, window.height);
    }

    vec2 VApp::windowGetSize() {
        double width = static_cast<double>(window.width);
        double height = static_cast<double>(window.height);
        return vec2{width, height};
    }

    // window fullscreen
    void VApp::windowSetFullScreen(bool fullScreen) {
        window.fullScreen = fullScreen;
    }

    bool VApp::windowGetFullScreen() {
        return window.fullScreen;
    }

    ///////////////////////////////////////////////////////////////

    // vulkan instance
    // create vulkan instance
    void VApp::vulkanInstanceCreate() {
        //vk app info
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = window.title;
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Kanji";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        //vk instance info
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        //setup glfw for vulkan
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;

        createInfo.enabledLayerCount = 0;

        //create the vk instance
        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
        //verify sucess
        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) 
            throw std::runtime_error("failed to create instance!");
        else 
            std::cout << "Vulkan instance created successfully!" << std::endl;

    }

    // create vulkan surface to draw on
    void VApp::vulkanSurfaceCreate() {
        if (glfwCreateWindowSurface(instance, window.glfwWindow, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create surface!");
        }
    }


    // vulkan device
    // vulkan create logical device
     void VApp::deviceCreate() {
        devicePickPhysicalDevice();
        deviceCreateLogicalDevice();
    }

    //pick graphics card (physical device) to use
    void VApp::devicePickPhysicalDevice() {
        vdevice.physicalDevice = VK_NULL_HANDLE;

        //listing physical devices
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (deviceCount == 0) 
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        //select suitable device
        int i = 0;
        for (auto device: devices) {
            if (deviceIsDeviceSuitable(device, i)) {
                vdevice.physicalDevice = device;
                i++;
            }
        }

        if (vdevice.physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        } else {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(vdevice.physicalDevice, &deviceProperties);
            std::cout << "Using Physical Device :" << deviceProperties.deviceName << std::endl;
        }
    }

    // check if device is suitable to be used
    bool VApp::deviceIsDeviceSuitable(VkPhysicalDevice device, int currentDeviceIndex) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        QueueFamilyIndices indices = deviceFindQueueFamilies(device);

        bool extensionsSupported = deviceCheckDeviceExtensionSupport(device);
        
        bool swapChainAdequate = false;
        
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = deviceQuerySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }
        bool isSuitable = extensionsSupported && indices.graphicsFamily.has_value() && indices.presentFamily.has_value();

        //
        if (currentDeviceIndex > 0)
            return isSuitable && deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        else 
            return isSuitable;
    }

    // check physical device support for extention
    bool VApp::deviceCheckDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(vdevice.deviceExtensions.begin(), vdevice.deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    //get swap chain query details
    SwapChainSupportDetails VApp::deviceQuerySwapChainSupport(VkPhysicalDevice device) {
        SwapChainSupportDetails details;
        //capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        //format
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        //prensent mode
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    //create logical device
    void VApp::deviceCreateLogicalDevice() {
        QueueFamilyIndices indices = deviceFindQueueFamilies(vdevice.physicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};

            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;

            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(vdevice.deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = vdevice.deviceExtensions.data();

        if (vkCreateDevice(vdevice.physicalDevice, &createInfo, nullptr, &(vdevice.device)) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(vdevice.device, indices.graphicsFamily.value(), 0, &(vdevice.graphicsQueue));
        vkGetDeviceQueue(vdevice.device, indices.presentFamily.value(), 0, &(vdevice.presentQueue));

    }

    //queue familes
    QueueFamilyIndices VApp::deviceFindQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            if (presentSupport) {
                indices.presentFamily = i;
            }

            i++;
        }

        return indices;
    }

    // vulkan swap chain
    // create the swap chain
    void VApp::swapChainCreate() {
        SwapChainSupportDetails swapChainSupport = deviceQuerySwapChainSupport(vdevice.physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = swapChainChooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = swapChainChooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = swapChainChooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        createInfo.oldSwapchain = VK_NULL_HANDLE;

        //creating the swap chain
        if (vkCreateSwapchainKHR(vdevice.device, &createInfo, nullptr, &(vswapChain.swapChain)) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(vdevice.device, vswapChain.swapChain, &imageCount, nullptr);
        vswapChain.swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(vdevice.device, vswapChain.swapChain, &imageCount, vswapChain.swapChainImages.data());

        vswapChain.swapChainImageFormat = surfaceFormat.format;
        vswapChain.swapChainExtent = extent;

    }

    //chose the right surface format
    VkSurfaceFormatKHR VApp::swapChainChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    //chose the right presentation mode
    VkPresentModeKHR VApp::swapChainChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_FIFO_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    //chose the swap extent
    VkExtent2D VApp::swapChainChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }

        int width, height;
        glfwGetFramebufferSize(window.glfwWindow, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        
        return actualExtent;

    }

    // image views
    // create image views
    void VApp::imageViewsCreate() {
        imageViews.resize(vswapChain.swapChainImages.size());

        for (size_t i = 0; i < vswapChain.swapChainImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = vswapChain.swapChainImages[i];

            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = vswapChain.swapChainImageFormat;

            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(vdevice.device, &createInfo, nullptr, &imageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }

    // destroy image views
    void VApp::imageViewDestroy() {
        for (auto imageView : imageViews) {
            vkDestroyImageView(vdevice.device, imageView, nullptr);
        }
    }

    // vulkan graphics pipeline
    // read file (used to read shaders bin)
    std::vector<char> VApp::readFile(const std::string& filePath) {
        std::ifstream file{filePath, std::ios::ate | std::ios::binary};

        if (!file.is_open()) {
            throw std::runtime_error("failed to load file " + filePath);
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);
        
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    };  


    // vulkan create pipeline
    void VApp::pipelineCreate(const std::string& vertFilePath, const std::string& fragFilePath){
        // read vertex shader file
        std::vector vertShaderCode = readFile(vertFilePath);
        std::cout << "vertex shader file size :" << vertShaderCode.size() << std::endl;
        // read fragment shader file
        std::vector fragShaderCode = readFile(fragFilePath);
        std::cout << "fragment shader file size :" << fragShaderCode.size() << std::endl;

        // create shader modules
        VkShaderModule vertShaderModule = pipelineCreateShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = pipelineCreateShaderModule(fragShaderCode);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        //dynamic states
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(vpipeline.dynamicStates.size());
        dynamicState.pDynamicStates = vpipeline.dynamicStates.data();

        //vertex input (later)
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

        //input assembly 
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        //viewport and scissors
        //viewport 
        VkViewport viewport{};
        VkRect2D scissor{};
            
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) vswapChain.swapChainExtent.width;
        viewport.height = (float) vswapChain.swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        //vkCmdSetViewport(*commandBuffer, 0, 1, &viewport);

        //scissors
        scissor.offset = {0, 0};
        scissor.extent = vswapChain.swapChainExtent;
        //vkCmdSetScissor(*commandBuffer, 0, 1, &scissor);

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;
        
        //rasterizer
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        //multisampling (later)
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        //color blending
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;

        //create pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        if (vkCreatePipelineLayout(vdevice.device, &pipelineLayoutInfo, nullptr, &(vpipeline.layout)) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        };  

        // create render pass
        renderPassCreate();

        //create the graphics pipeline
        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;

        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;

        pipelineInfo.layout = vpipeline.layout;

        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;

        if (vkCreateGraphicsPipelines(vdevice.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &(vpipeline.graphics)) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(vdevice.device, fragShaderModule, nullptr);
        vkDestroyShaderModule(vdevice.device, vertShaderModule, nullptr);
    }

    //create shader modules
    VkShaderModule VApp::pipelineCreateShaderModule(const std::vector<char>& code) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(vdevice.device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }

    //create render pass
    void VApp::renderPassCreate() {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = vswapChain.swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        //stencil (later)
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        //initial layout (more to know later)
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        //sub pass (only one for now)
        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        //render pass create info
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;

        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;

        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        //create render pass
        if (vkCreateRenderPass(vdevice.device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }

    }

    // vulkan destroy pipeline
    void VApp::pipelineDestroy() {
        // nothing for now
    }

    // vulkan buffers
    // vulkan frame buffer
    // vulkan create frame buffers
    void VApp::frameBuffersCreate() {
        frameBuffers.resize(imageViews.size());

        for (size_t i = 0; i < imageViews.size(); i++) {
            VkImageView attachments[] = {
                imageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = vswapChain.swapChainExtent.width;
            framebufferInfo.height = vswapChain.swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(vdevice.device, &framebufferInfo, nullptr, &frameBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }

    }
    // destroy frame buffer
    void VApp::frameBufferDestroy() {
        for (auto framebuffer : frameBuffers) {
            vkDestroyFramebuffer(vdevice.device, framebuffer, nullptr);
        }
    }

    // command pool
    // create command pool
    void VApp::commandPoolCreate() {
        QueueFamilyIndices queueFamilyIndices = deviceFindQueueFamilies(vdevice.physicalDevice);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(vdevice.device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
        }
    }
    // destroy command pool
    void VApp::commandPoolDestroy() {
        vkDestroyCommandPool(vdevice.device, commandPool, nullptr);
    }

    // command buffer
    // create command buffer 
    void VApp::commandBufferCreate() {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(vdevice.device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    // record command buffer
    void VApp::commandBufferRecord(uint32_t imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        //begin render pass
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = frameBuffers[imageIndex];

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = vswapChain.swapChainExtent;

        VkClearValue clearColor{{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);      

        //bind pipeline
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vpipeline.graphics);


        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(vswapChain.swapChainExtent.width);
        viewport.height = static_cast<float>(vswapChain.swapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = vswapChain.swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffer);
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    // vulkan sync objects
    // create vulkan sync objects
    void VApp::syncObjectsCreate() {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        //create objects
        if (vkCreateSemaphore(vdevice.device, &semaphoreInfo, nullptr, &syncObjects.imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(vdevice.device, &semaphoreInfo, nullptr, &syncObjects.renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(vdevice.device, &fenceInfo, nullptr, &syncObjects.inFlightFence) != VK_SUCCESS) {
            throw std::runtime_error("failed to create sync objects!");
        }
    }

    // destroy vulkan sync objects
    void VApp::syncObjectsDestroy() {
        vkDestroySemaphore(vdevice.device, syncObjects.imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(vdevice.device, syncObjects.renderFinishedSemaphore, nullptr);
        vkDestroyFence(vdevice.device, syncObjects.inFlightFence, nullptr);
    }

    /////////////////////////////////////////////////////////////

    // draw frame in the window
    void VApp::drawFrame() {
        //wait for previous frame to finish render
        vkWaitForFences(vdevice.device, 1, &syncObjects.inFlightFence, VK_TRUE, UINT64_MAX);
        //reset fence
        vkResetFences(vdevice.device, 1, &syncObjects.inFlightFence);
        

        //aquaring an image from the swap chain
        uint32_t imageIndex;
        auto result = vkAcquireNextImageKHR(vdevice.device, vswapChain.swapChain, UINT64_MAX, syncObjects.imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

        //std::cout << imageIndex << std::endl;
        //record command buffer
        vkResetCommandBuffer(commandBuffer, 0);
        commandBufferRecord(imageIndex);
        
        //submit command buffer
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {syncObjects.imageAvailableSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        
        VkSemaphore signalSemaphores[] = {syncObjects.renderFinishedSemaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;


        if (vkQueueSubmit(vdevice.graphicsQueue, 1, &submitInfo, syncObjects.inFlightFence) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {vswapChain.swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        vkQueuePresentKHR(vdevice.presentQueue, &presentInfo);

    }


    //init vulkan app
    void VApp::init() {
        // initiate window for vulkan app
        windowInit();
        // create a vulkan istance
        vulkanInstanceCreate();
        // create vulkan surface
        vulkanSurfaceCreate();
        // create vulkan device
        deviceCreate();
        // create swap chain
        swapChainCreate();
        // create image views
        imageViewsCreate();
        // create vulkan pipeline
        pipelineCreate("./shaders/main.vert.spv", "./shaders/main.frag.spv");
        // create buffers
        frameBuffersCreate();
        commandPoolCreate();
        commandBufferCreate();
        // create sync objects
        syncObjectsCreate();

    }

    // start vulkan app
    // pass update function as parameter
    void VApp::start(void (*update)(double delta)) {
        while(!glfwWindowShouldClose(window.glfwWindow)){
            glfwPollEvents();
            drawFrame();
            // call update function
            update(0.0);
        }
    }

    // destroy vulkan app
    void VApp::destroy() {
        // destroy sync objects
        syncObjectsDestroy();
        // destroy buffers
        commandPoolDestroy();
        frameBufferDestroy();
        // destroy vulkan pipeline
        pipelineDestroy();
        // destroy image views
        imageViewDestroy();
        // destroy swap chain
        vkDestroySwapchainKHR(vdevice.device, vswapChain.swapChain, nullptr);
        // destroy vulkan device
        vkDestroyDevice(vdevice.device, nullptr);
        // destroy vulkan surface
        vkDestroySurfaceKHR(instance, surface, nullptr);
        //destroy vulkan instance
        vkDestroyInstance(instance, nullptr);
        
        //destroy window
        glfwDestroyWindow(window.glfwWindow);
        glfwTerminate();
    }

}