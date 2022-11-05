#include <iostream>
#include "kanji.h"


Kanji::KanjiGame game{};


int main() {
    game.start();
    game.destroy();
    
    return 0;
}