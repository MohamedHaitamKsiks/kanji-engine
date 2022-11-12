#pragma once

#include <cmath>
#include <iostream>
#include "vector.h"

//matrices
namespace Kanji {
    //mat3
    typedef struct mat3 {
        float data[3][3];

        static mat3 basis() {
            mat3 b;
            for (int j = 0; j < 3; j++) {
                for (int i = 0; i < 3; i++) {
                    if (i == j)
                        b.data[i][j] = 1.0f;
                    else
                        b.data[i][j] = 0.0f;
                }
            }
            return b;
        }

        static mat3 rotation(float angle) {
            mat3 b = basis();
            b.data[0][0] = cos(angle);
            b.data[0][1] = -sin(angle);
            b.data[1][0] = sin(angle);
            b.data[1][1] = cos(angle);
            return b;
        }

        static void print(mat3 m) {
            for (int j = 0; j < 3; j++) {
                for (int i = 0; i < 3; i++) {
                    std::cout<< m.data[j][i] << ',';
                }
                std::cout << std::endl;
            }
            
        }
    } mat3;

    //mat4
    typedef struct mat4 {
        float data[4][4];
    } mat4;


    
}


