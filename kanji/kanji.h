
/*  
    #Description
        Kanji is a free open source game engine
    made by Ksiks Mohamed Haitam as a personal project to 
    learn more about game engine development.

    #Features:

        1-renderer (we are using the vulkan API)

        2-core
            a-Memory managment
            b-Async files i/o
            c-Math class (vectors, transformation matrices ...)
            d-Threads (later)
            e-StringID

        3-EntityComponentSystem (ECS)

        4-scenes

        5-audio

        6-physics

*/

#pragma once

#include "renderer/vapp.h"
#include "core/memory/allocator.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>



namespace Kanji {
    
    // a kajin engine class
    class KanjiGame {
        
        public:
            // vapp
            VApp app{};
            // start kanji game
            void start();
            // destroy kanji game
            void destroy();


    };

}