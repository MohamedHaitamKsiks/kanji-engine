#include "kanji.h"
#include "core/time/time.h"
#include "renderer/mesh/mesh.h"

#include <cstdio>


namespace Kanji {

    // start kanji game
    void KanjiGame::start() {

        //create window
        window.create();
        // create vulkan context
        vcontext.init(&window);
        // create renderer
        renderer.init(&vcontext);

        std::vector<Vertex> vertices = {
            {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f}},
            {{1.0f, -1.0f, -1.0f}, {1.0f, 0.0f}},
            {{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f}},
            {{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f}},
            {{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f}},
            {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
            {{-1.0f, 1.0f, 1.0f}, {0.0f, 0.1f}},
        };

        std::vector<uint16_t> indices = {
            0, 1, 3, 3, 1, 2,
            1, 5, 2, 2, 5, 6,
            5, 4, 6, 6, 4, 7,
            4, 0, 7, 7, 0, 3,
            3, 2, 7, 7, 2, 6,
            4, 5, 0, 0, 5, 1
        };

        Mesh mesh1, mesh2;

        mesh1 = renderer.meshLoad(vertices, indices);
        mesh2 = renderer.meshLoad(vertices, indices);


        //craete material
        /*
        Material material = renderer.materialCreate(
            "shaders/mat.vert.spv",
            "shaders/mat.frag.spv"
        );
        renderer.meshSetMaterial(mesh1, material);
        */

        MeshInstance* instance = renderer.meshInstanceCreate(mesh1);

        MeshInstance* instance2 = renderer.meshInstanceCreate(mesh2);
        float s = 1.0f;
        instance2->transform = mat4::ortho(-s, s, 1.0, s, -s, 10.0) * mat4::perspective(1.0, 5.0) * mat4::translate(vec3{5.0, 0.0, 5.0});

        double delta = 0.0;
        float angle = 0.0;
        //start main loop
        while (!window.isClosed()) {
            double startTime = Time::now();
            glfwPollEvents();
            renderer.drawFrame();
            angle += 10.0 * delta;
            
            instance->transform = mat4::ortho(-s, s, 1.0, s, -s, 10.0) * mat4::perspective(1.0, 5.0)
            * mat4::translate(vec3{0.0, 0.0, 5.0})
            * mat4::rotationY(0.5*angle) * mat4::rotationZ(2.0 * angle) * mat4::rotationX(angle) ;
            delta = Time::now() - startTime;
            char title[64];
            sprintf(title, "%lf FPS", round( 1.0 / delta));
            window.setTitle(title);
        }

    }


    // destroy kanji game
    void KanjiGame::destroy() {
        vcontext.destroy();
    }

}