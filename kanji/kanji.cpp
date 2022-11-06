#include "kanji.h"
#include "renderer/vertex.h"

// update functions
void update(double delta) {
    // std::cout << "Test" << std::endl;
}

namespace Kanji {

    // start kanji game
    void KanjiGame::start() {
        std::cout << sizeof(vertex) << std::endl;
        // init vapp
        app.init();
        
        //start vapp
        app.start(&update);
    }

    // destroy kanji game
    void KanjiGame::destroy() {
        app.destroy();
    }

}