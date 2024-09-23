#include "objects.h"
#include "resources.h"

int Bucket::clicked(int x, int y) {
    return (x > 0 && y > 0 && x < width && y < height);
}

void Bucket::draw() {
    if (game.grabbed != game.BUCKET)
        ui.draw(picture, 0, 0);
}

void Bucket::load_pix() {
    picture.load(GFX_RES_EXTRAS, GFX_RES_EXTRAS_WIDTH, GFX_RES_EXTRAS_HEIGHT);
    width = picture.width;
    height = picture.height;
}

void Bucket::release_pix() {
    picture.release();
}
