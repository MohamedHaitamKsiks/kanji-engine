#pragma once

#include <cstring>
#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vdevice.h"

/*
    When using the Kanji VkBuffer always:
    1- push first the data that won't be 
    deleted alot since the delete operation is slow
*/
namespace Kanji {

    class Buffer {

        private:
            void* data;

        public:
            // vdevice
            VDevice* vdevice;

            // buffer data
            VkBuffer buffer;
            VkDeviceSize size;
            VkDeviceMemory memory;
            uint16_t head = 0;

            //buffer methods
            static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VDevice* vdevice);
            void create(VDevice* _vdevice, size_t bufferSize);
            void destroy();
            void push(const void* bufferData, const size_t size);
            void remove(const uint16_t index, const size_t size);
            void clear();

    };

}