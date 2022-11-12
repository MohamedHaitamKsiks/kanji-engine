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
            {{0.0f, -0.2f}, {0.0f, 0.5f, 0.5f}},
            {{0.4f, -0.2f}, {0.0f, 0.0f, 0.5f}},
            {{0.4f, 0.2f}, {0.0f, 0.5f, 0.5f}},
            {{0.0f, 0.2f}, {0.0f, 0.0f, 0.5f}}
        };

        std::vector<uint16_t> indices = {
            0, 1, 2,
            2, 3, 0
        };

        Mesh mesh1 = app.meshLoad(vertices, indices);

        std::vector<Vertex> vertices2 = {
            {{-0.4f, -0.2f}, {0.0f, 0.5f, 0.0f}},
            {{0.0f, -0.2f}, {0.0f, 0.5f, 0.0f}},
            {{0.0f, 0.2f}, {0.0f, 0.5f, 0.0f}},
            {{-0.4f, 0.2f}, {0.0f, 0.5f, 0.0f}}
        };

        std::vector<uint16_t> indices2 = {
            0, 1, 2,
            2, 3, 0
        };

        Mesh mesh2 = app.meshLoad(vertices2, indices2);
        

        MeshInstance* instance = app.meshInstanceCreate(mesh1);
        instance->transform = mat3::rotation(M_PI / 4.0);

        MeshInstance* instance2 = app.meshInstanceCreate(mesh2);

        std::cout<< mesh1 << " " << mesh2 << std::endl;
        //app.meshFree(info);

        std::cout << sizeof(Kanji::PushConstant) << std::endl;

        //start vapp
        app.start(&update);
    }

    // destroy kanji game
    void KanjiGame::destroy() {
        app.destroy();
    }

}