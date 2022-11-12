#pragma once


#include <cstdlib>
#define CHUNK_END -1;

namespace Kanji {
    
    typedef unsigned int ChunkId;

    template<typename T>
    class PoolAllocator {
        public:
            //init pool allocator
            void init(unsigned int poolSize) {
                //set sizes
                maxSize = (size_t)poolSize;
                size = 0;

                // allocate space for data and 
                data = (T*) (malloc(sizeof(T) * poolSize));
                freeChunkNext = (ChunkId*) (malloc(sizeof(ChunkId) * poolSize));
                usedChunks = (bool*) (malloc(sizeof(bool) * poolSize));
                //prepare freechunk linked list
                for (ChunkId i = 0; i < poolSize; i++) {
                    freeChunkNext[i] = (i + 1) % maxSize;
                    usedChunks[i] = false;
                }

            };
            // returns the index of the allocated ressource
            ChunkId alloc() {
                // get index
                ChunkId index = freeHead;
                //
                usedChunks[index] = true;

                //move head to the next node
                freeHead = freeChunkNext[freeHead];
                // increase size
                size++;
                return index;
            };
            // free index
            void free(ChunkId index) {
                freeChunkNext[index] = freeHead; 
                freeHead = index;
                usedChunks[index] = false;
            };

            //is chunk used
            bool isUsed(ChunkId index) {
                return usedChunks[index];
            }

            // get data at position
            T* get(ChunkId index) {
                return data + index;
            };

            //get sizes
            size_t getMaxSize() {
                return maxSize;
            };
            size_t getSize() {
                return size;
            };


        private:
            T* data;
            size_t maxSize;
            size_t size;
            // linked list of all free chunks 
            ChunkId freeHead = 0;
            ChunkId* freeChunkNext;
            bool* usedChunks;
    };

}