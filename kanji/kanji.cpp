#include "kanji.h"
#include "renderer/vertex.h"
#include "core/time/time.h"
#include "renderer/mesh.h"



// update functions
void update(double delta) {
    //std::cout << sizeof(Kanji::PushConstant) << std::endl;
    
}

namespace Kanji {

    // start kanji game
    void KanjiGame::start() {
        std::cout << sizeof(Vertex) << std::endl;
        // init vapp
        app.init();

        
        std::vector<Vertex> vertices = {
            {{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}},
            {{1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
            {{1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, 0.1f}},
            {{-1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}},
            {{-1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},
            {{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
            {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.5f}},
            {{-1.0f, 1.0f, 1.0f}, {0.5f, 0.5f, 0.5f}},
        };

        std::vector<uint16_t> indices = {
            0, 1, 3, 3, 1, 2,
            1, 5, 2, 2, 5, 6,
            5, 4, 6, 6, 4, 7,
            4, 0, 7, 7, 0, 3,
            3, 2, 7, 7, 2, 6,
            4, 5, 0, 0, 5, 1
        };


        Mesh mesh1 = app.meshLoad(vertices, indices);

        Kanji::MeshInstance* instance = app.meshInstanceCreate(mesh1);
        mat4::print(instance->transform);

        std::cout << sizeof(Kanji::PushConstant) << std::endl;

        //start vapp
        app.start(&update);
    }

    // destroy kanji game
    void KanjiGame::destroy() {
        app.destroy();
    }

}