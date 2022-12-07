#pragma once

#include "thirdparty/stb_image.h"
#define STB_IMAGE_IMPLEMENTATION

#include<cstdlib>
#include<stdexcept>

namespace Kanji {
    //image definition
    struct Image {
        char* data;
        int width;
        int height;
        //image load
        static Image load(const char* path) {
            Image image;
            image.data = (char*) stbi_load(path, &(image.width), &(image.height), nullptr, STBI_rgb_alpha);

            if (!image.data) {
                throw std::runtime_error("failed to load texture image!");
            }
            
            return image;
        };

        //image free
        static void free(Image image) {
            std::free(image.data);
        };
    };

}

