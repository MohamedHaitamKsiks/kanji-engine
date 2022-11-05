#include "kanji.h"

// update functions
void update(double delta) {
    // std::cout << "Test" << std::endl;
}

namespace Kanji {

    // start kanji game
    void KanjiGame::start() {
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