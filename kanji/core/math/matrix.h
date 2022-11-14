#pragma once

#include <cmath>
#include <iostream>
#include "vector.h"

//matrices
namespace Kanji {
    //mat4
    typedef struct mat4 {
        float data[4][4];

        // get identity matrix
        static mat4 identity() {
            mat4 b;
            for (int j = 0; j < 4; j++) {
                for (int i = 0; i < 4; i++) {
                    if (i == j)
                        b.data[i][j] = 1.0f;
                    else
                        b.data[i][j] = 0.0f;
                }
            }
            return b;
        }

        // get zero matrix
        static mat4 zeros() {
            mat4 b;
            for (int j = 0; j < 4; j++) {
                for (int i = 0; i < 4; i++) {
                    b.data[i][j] = 0.0f;
                }
            }
            return b;
        }

        // scale
        static mat4 scale(float s) {
            mat4 b = identity();
            for (int i = 0; i < 3; i++) {
                b.data[i][i] = s;
            }
            return b;
        }

        //rotation X
        static mat4 rotationX(float angle) {
            mat4 b = identity();
            b.data[1][1] = cos(angle);
            b.data[1][2] = -sin(angle);
            b.data[2][1] = sin(angle);
            b.data[2][2] = cos(angle);
            return b;
        }

        //rotation Y
        static mat4 rotationY(float angle) {
            mat4 b = identity();
            b.data[0][0] = cos(angle);
            b.data[0][2] = -sin(angle);
            b.data[2][0] = sin(angle);
            b.data[2][2] = cos(angle);
            return b;
        }

        //translate 
        static mat4 translate(vec3 v) {
            mat4 b = identity();
            b.data[0][3] = v.x;
            b.data[1][3] = v.y;
            b.data[2][3] = v.z;
            return b;
        }

        // matrix orthographic transoform
        static mat4 ortho(float left, float bottom, float near, float right, float top, float far) {
            mat4 mOrth = identity();

            mOrth.data[0][0] = 2.0 / (right - left);
            mOrth.data[1][1] = 2.0 / (bottom - top);
            mOrth.data[2][2] = 1.0 / (far - near);

            mOrth.data[0][3] = - (right + left) / (right - left);
            mOrth.data[1][3] = - (bottom + top) / (bottom - top);
            mOrth.data[2][3] =   - near / (far - near);

            return mOrth;
        }

        //matrix perspective transform
        static mat4 perspective(float near, float far) {
            mat4 mPerspective = zeros();

            mPerspective.data[0][0] = near;
            mPerspective.data[1][1] = near;
            mPerspective.data[2][2] = far + near;
            mPerspective.data[2][3] = - far * near;
            mPerspective.data[3][2] = 1.0f;

            return mPerspective;
        }


        //rotation Z
        static mat4 rotationZ(float angle) {
            mat4 b = identity();
            b.data[0][0] = cos(angle);
            b.data[0][1] = -sin(angle);
            b.data[1][0] = sin(angle);
            b.data[1][1] = cos(angle);
            return b;
        }

        // matrix multiplication
        mat4 operator * (mat4 m) {
            mat4 res = *this;
            for (int j = 0; j < 4; j++) {
                for (int i = 0; i < 4; i++) {
                    res.data[j][i] = 0.0f;
                    for (int k = 0; k < 4; k++) {
                        res.data[j][i] += data[j][k] * m.data[k][i];
                    }
                }
            }
            return res;
        }


        // matrix addition
        mat4 operator + (mat4 m) {
            mat4 res;

            for (int j = 0; j < 4; j++) {
                for (int i = 0; i < 4; i++) {
                    res.data[j][i] = data[j][i] + m.data[j][i];
                    
                }
            }
            return res;
        }


        //print matrix 
        static void print(mat4 m) {
            for (int j = 0; j < 4; j++) {
                for (int i = 0; i < 4; i++) {
                    std::cout<< m.data[j][i] << ',';
                }
                std::cout << std::endl;
            }
            
        }
    } mat4;


    
}


