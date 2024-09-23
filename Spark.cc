#include "objects.h"
#include "pics.h"

int Spark::delay(unsigned int lev) {
	return game.MAX (20-lev, 0);
}

void Spark::load_pix() {
	int i;
	for (i=0; i<2; i++)
		pictures[i].load(extras_pic[i+1],extras_mask[i+1],EXTRAS_WIDTH,EXTRAS_HEIGHT);
	width = pictures[0].width;
	height = pictures[0].height;
}
