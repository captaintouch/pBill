#include "objects.h"
#include "resources.h"

int Spark::delay(unsigned int lev) {
    return game.MAX(20 - lev, 0);
}

void Spark::load_pix() {
    int i;
    for (i = 0; i < 2; i++)
        pictures[i].load(GFX_RES_EXTRAS + i + 1, GFX_RES_EXTRAS_WIDTH, GFX_RES_EXTRAS_HEIGHT);
    width = pictures[0].width;
    height = pictures[0].height;
}

void Spark::release_pix() {
    int i;
    for (i = 0; i < 2; i++)
        pictures[i].release();
}
