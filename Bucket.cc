#include "objects.h"
#include "pics.h"

int Bucket::clicked (int x, int y) {
	return (x > 0 && y > 0 && x < width && y < height);
}

void Bucket::draw() {
	if (game.grabbed != game.BUCKET) ui.draw(picture, 0, 0);
}

void Bucket::load_pix() {
	picture.load(extras_pic[0],extras_mask[0],EXTRAS_WIDTH,EXTRAS_HEIGHT);
	width = picture.width;
	height = picture.height;
}
