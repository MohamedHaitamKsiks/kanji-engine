#include "buffer.h"

namespace Kanji {
    // vulkan buffers
    uint32_t Buffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VDevice* vdevice) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(vdevice->physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
        return UINT32_MAX;

    }

    // create vertex buffer
    void Buffer::create(VDevice* _vdevice, size_t bufferSize) {
        //get vdevice
        vdevice = _vdevice;
        //create buffer
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferSize;
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        size = bufferInfo.size;

        if (vkCreateBuffer(vdevice->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vertex buffer!");
        }

        //allocate memory to the buffer
        //memory requirment
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(vdevice->device, buffer, &memRequirements);
        // allocate memory
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vdevice);
        if (vkAllocateMemory(vdevice->device, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate vertex buffer memory!");
        }
        vkBindBufferMemory(vdevice->device, buffer, memory, 0);
        
    }

    // push data to buffer memory
    void Buffer::push(const void* bufferData, const size_t size) {
        void* data;
        vkMapMemory(vdevice->device, memory, head, (VkDeviceSize) size, 0, &data);
        memcpy(data, bufferData, size);
        vkUnmapMemory(vdevice->device, memory);
        head += (uint16_t) size;
    }

    //remove from buffer
    void Buffer::remove(const uint16_t index, const size_t size) {
        void* data;
        if (index + size < head) {
            vkMapMemory(vdevice->device, memory, index, (VkDeviceSize) (size - index), 0, &data);
            memcpy(data, (void*) ((char*)data + size), (size - index - size));
            vkUnmapMemory(vdevice->device, memory);
        }
        head -= (uint16_t) size;
    }

    // destroy vertex buffer
    void Buffer::destroy() {
        vkDestroyBuffer(vdevice->device, buffer, nullptr);
        vkFreeMemory(vdevice->device, memory, nullptr);
    }

    // clear buffer
    void Buffer::clear() {
        head = 0;
    }


}
