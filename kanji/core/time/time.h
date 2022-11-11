#pragma once

#include <ctime>

namespace Kanji {

    struct Time {
        static double now() {
            return (double) (clock() / CLOCKS_PER_SEC) + 
            (double) (clock() % CLOCKS_PER_SEC) / (double) CLOCKS_PER_SEC;
        };
    };

}