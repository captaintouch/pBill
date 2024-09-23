#include "objects.h"
#include "pics.h"

void Library::load_pix() {
	int i;
	for (i=0; i <= NUM_OS; i++) {
		os[i].load(os_pic[i],NONE,OS_WIDTH,OS_HEIGHT);
	}
	width = os[0].width;
	height = os[0].height;
}
