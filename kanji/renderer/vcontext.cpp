#include "vcontext.h"


namespace Kanji {

    // vulkan instance
    // create vulkan instance
    void VContext::vulkanInstanceCreate() {
        //vk app info
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Vulkan App";
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
    void VContext::vulkanSurfaceCreate() {
        if (glfwCreateWindowSurface(instance, window->getGLFWwindow(), nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create surface!");
        }
    }


    // vulkan device
    // vulkan create logical device
     void VContext::deviceCreate() {
        devicePickPhysicalDevice();
        deviceCreateLogicalDevice();
    }

    //pick graphics card (physical device) to use
    void VContext::devicePickPhysicalDevice() {
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
    bool VContext::deviceIsDeviceSuitable(VkPhysicalDevice device, int currentDeviceIndex) {
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
    bool VContext::deviceCheckDeviceExtensionSupport(VkPhysicalDevice device) {
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
    SwapChainSupportDetails VContext::deviceQuerySwapChainSupport(VkPhysicalDevice device) {
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
    void VContext::deviceCreateLogicalDevice() {
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
    QueueFamilyIndices VContext::deviceFindQueueFamilies(VkPhysicalDevice device) {
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
    void VContext::swapChainCreate() {
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
    VkSurfaceFormatKHR VContext::swapChainChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    //chose the right presentation mode
    VkPresentModeKHR VContext::swapChainChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_FIFO_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    //chose the swap extent
    VkExtent2D VContext::swapChainChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }

        int width, height;
        glfwGetFramebufferSize(window->getGLFWwindow(), &width, &height);

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
    void VContext::imageViewsCreate() {
        imageViews.resize(vswapChain.swapChainImages.size());

        for (size_t i = 0; i < vswapChain.swapChainImages.size(); i++) {
            imageViews[i] = imageViewCreate(vswapChain.swapChainImages[i], vswapChain.swapChainImageFormat);
        }
    }

    // destroy image views
    void VContext::imageViewDestroy() {
        for (auto imageView : imageViews) {
            imageViewDestroy(imageView);
        }
    }

    // vulkan graphics pipeline
    // read file (used to read shaders bin)
    std::vector<char> VContext::readFile(const std::string& filePath) {
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
    Pipeline VContext::pipelineCreate(const std::string& vertFilePath, const std::string& fragFilePath){
        Pipeline pipeline;  
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
        dynamicState.dynamicStateCount = static_cast<uint32_t>(pipeline.dynamicStates.size());
        dynamicState.pDynamicStates = pipeline.dynamicStates.data();
        
        //vertex input (now)
        // get vertex descritoion
        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();
        //fill vertex input info
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription; // pass biding description
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data(); // pass attribute description

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

        
        if (vkCreatePipelineLayout(vdevice.device, &pipelineLayoutInfo, nullptr, &(pipeline.layout)) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        };  

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
    

        pipelineInfo.layout = pipeline.layout;

        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;   

        if (vkCreateGraphicsPipelines(vdevice.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &(pipeline.graphics)) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(vdevice.device, fragShaderModule, nullptr);
        vkDestroyShaderModule(vdevice.device, vertShaderModule, nullptr);
        
        return pipeline;
    }

    //create shader modules
    VkShaderModule VContext::pipelineCreateShaderModule(const std::vector<char>& code) {
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

    // bind pipeline to command buffer
    void VContext::pipelineBind(Pipeline pipeline) {
        //bind pipeline
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.graphics);
    }

    //create render pass
    void VContext::renderPassCreate() {
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
    void VContext::pipelineDestroy(Pipeline pipeline) {
        // nothing for now
    }



    // vulkan frame buffer
    // vulkan create frame buffers
    void VContext::frameBuffersCreate() {
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
    void VContext::frameBufferDestroy() {
        for (auto framebuffer : frameBuffers) {
            vkDestroyFramebuffer(vdevice.device, framebuffer, nullptr);
        }
    }

    // command pool
    // create command pool
    void VContext::commandPoolCreate() {
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
    void VContext::commandPoolDestroy() {
        vkDestroyCommandPool(vdevice.device, commandPool, nullptr);
    }

    // command buffer
    // create command buffer 
    void VContext::commandBufferCreate() {
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
    // start command buffer record
    void VContext::commandBufferStartRecord(uint32_t imageIndex) {
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
        
    }

    // command buffer end record
    void VContext::commandBufferEndRecord() {
        //end render pass
        vkCmdEndRenderPass(commandBuffer);
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    // signle time commands
    VkCommandBuffer VContext::singleTimeCommandsBegin() {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer singleTimeCommandBuffer;
        vkAllocateCommandBuffers(vdevice.device, &allocInfo, &singleTimeCommandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(singleTimeCommandBuffer, &beginInfo);

        return singleTimeCommandBuffer;
    }

    void VContext::singleTimeCommandsEnd(VkCommandBuffer singleTimeCommandBuffer) {
        vkEndCommandBuffer(singleTimeCommandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &singleTimeCommandBuffer;

        vkQueueSubmit(vdevice.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(vdevice.graphicsQueue);

        vkFreeCommandBuffers(vdevice.device, commandPool, 1, &singleTimeCommandBuffer);
    }

    // vulkan sync objects
    // create vulkan sync objects
    void VContext::syncObjectsCreate() {
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
    void VContext::syncObjectsDestroy() {
        vkDestroySemaphore(vdevice.device, syncObjects.imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(vdevice.device, syncObjects.renderFinishedSemaphore, nullptr);
        vkDestroyFence(vdevice.device, syncObjects.inFlightFence, nullptr);
    }

    /////////////////////////////////////////////////////////////

    // draw frame in the window
    void VContext::drawFrameStart(uint32_t* imageIndex) {
        //wait for previous frame to finish render
        vkWaitForFences(vdevice.device, 1, &syncObjects.inFlightFence, VK_TRUE, UINT64_MAX);
        //reset fence
        vkResetFences(vdevice.device, 1, &syncObjects.inFlightFence);
        //aquaring an image from the swap chain
        auto result = vkAcquireNextImageKHR(vdevice.device, vswapChain.swapChain, UINT64_MAX, syncObjects.imageAvailableSemaphore, VK_NULL_HANDLE, imageIndex);
        //reset command buffer and start record
        vkResetCommandBuffer(commandBuffer, 0);
        commandBufferStartRecord(*imageIndex);
    }

    // end draw frame in the window
    void VContext::drawFrameEnd(uint32_t* imageIndex) {
        //end command buffer record
        commandBufferEndRecord(); 
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
        presentInfo.pImageIndices = imageIndex;

        vkQueuePresentKHR(vdevice.presentQueue, &presentInfo);

    }

    // mesh draw
    void VContext::meshDraw(MeshInfo meshInfo, PushConstant* pushConstant, Pipeline pipeline) {
        vkCmdPushConstants(commandBuffer, pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstant), pushConstant);
        vkCmdDrawIndexed(commandBuffer, meshInfo.indexBufferSize, 1, 0, 0, 0);
    }

    // mesh bind
    // always bind before draw
    void VContext::meshBind(MeshInfo meshInfo) {
        VkBuffer vertexBuffers[] = {vertexBuffer.buffer};
        VkDeviceSize offsets[] = {meshInfo.vertexBufferIndex};
        // bind the vertex buffer
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        // bind index buffer
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer.buffer, meshInfo.indexBufferIndex, VK_INDEX_TYPE_UINT16);
    }

    //image
    //image create
    void VContext::imageCreate(TextureInfo* textureInfo) {
        //image create info
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = textureInfo->width;
        imageInfo.extent.height = textureInfo->height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = textureInfo->format; //image format (let's go with 4 bytes RGBA)
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;


        //create the image
        if (vkCreateImage(vdevice.device, &imageInfo, nullptr, &(textureInfo->image)) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        //allocate memory for the image
        //get memory requirements
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(vdevice.device, textureInfo->image, &memRequirements);

        //memory allocate info
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = Buffer::findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vdevice);
        //memort allocate
        if (vkAllocateMemory(vdevice.device, &allocInfo, nullptr, &(textureInfo->imageMemory)) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }
        //bind image to memroy
        vkBindImageMemory(vdevice.device, textureInfo->image, textureInfo->imageMemory, 0);

    }

    //image layout transition between different layouts
    void VContext::imageLayoutTransition(TextureInfo* textureInfo, VkImageLayout oldLayout, VkImageLayout newLayout) {
        // start single time command buffer
        VkCommandBuffer singleTimeCommandBuffer = singleTimeCommandsBegin();
        //create image memory barrier
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        // ignore family queue
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        //image info
        barrier.image = textureInfo->image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        //access masks
        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } 
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } 
        else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        //submit pipeline barrier
        vkCmdPipelineBarrier(singleTimeCommandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 0, &barrier);
        //end command buffer
        singleTimeCommandsEnd(singleTimeCommandBuffer);
    }

    //create image view
    VkImageView VContext::imageViewCreate(VkImage image, VkFormat format) {
        VkImageView imageView;
        //image view create info
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = image;

        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = format;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(vdevice.device, &createInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }

        return imageView;

    }

    //destroy image view
    void VContext::imageViewDestroy(VkImageView imageView) {
        vkDestroyImageView(vdevice.device, imageView, nullptr);
    }

    //copy data from image buffer to image
    void VContext::imageCopyBufferToImage(TextureInfo* textureInfo) {
        // start single time command buffer
        VkCommandBuffer singleTimeCommandBuffer = singleTimeCommandsBegin();
        //specifie image region
        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            textureInfo->width,
            textureInfo->height,
            1
        };

        //exeute copy command from buffer to image
        vkCmdCopyBufferToImage(
            singleTimeCommandBuffer,
            imageBuffer.buffer,
            textureInfo->image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );

        //end command buffer
        singleTimeCommandsEnd(singleTimeCommandBuffer);
    }

    //destroy image
    void VContext::imageDestroy(TextureInfo* textureInfo) {
        vkDestroyImage(vdevice.device, textureInfo->image, nullptr);
        vkFreeMemory(vdevice.device, textureInfo->imageMemory, nullptr);
    }

    //image sampler
    void VContext::imageSamplerCreate(TextureInfo* textureInfo) {
        //sampler create info
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        //filter for near and far texels
        samplerInfo.magFilter = VK_FILTER_NEAREST;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        //adressing mode
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        //get physical device properties
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(vdevice.physicalDevice, &properties);
        //set antistropy
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        //outsite texture color when nothing
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        //use normalised cordinates
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        //comp (later!!!)
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        //minimap (later!!!)
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;
        //create the sampler
        if (vkCreateSampler(vdevice.device, &samplerInfo, nullptr, &(textureInfo->sampler)) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
        
    }

    //destroy image sampler
    void VContext::imageSamplerDestroy(TextureInfo* textureInfo) {
        vkDestroySampler(vdevice.device, textureInfo->sampler, nullptr);
    }


    //init vulkan app
    void VContext::init(Window* _window) {
        // initiate window for vulkan app
        window = _window;
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
        // create render pass
        renderPassCreate();
        // create buffers
        frameBuffersCreate();
        commandPoolCreate();
        // create vertex buffer
        vertexBuffer.create(&vdevice, VERTEX_BUFFER_SIZE);
        // create index buffer
        indexBuffer.create(&vdevice, INDEX_BUFFER_SIZE);
        // create image buffer
        imageBuffer.create(&vdevice, IMAGE_BUFFER_SIZE);
        // create command buffer
        commandBufferCreate();
        // create sync objects
        syncObjectsCreate();

    }

    // destroy vulkan app
    void VContext::destroy() {
        // destroy sync objects
        syncObjectsDestroy();
        // destroy buffers
        vertexBuffer.destroy();
        indexBuffer.destroy();
        imageBuffer.destroy();
        commandPoolDestroy();
        frameBufferDestroy();
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
    }

}