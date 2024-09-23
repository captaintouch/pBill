#include "objects.h"
#include "drawhelper.h"
#include "resources.h"

void Library::load_pix() {
	int i;
	for (i=0; i < GFX_RES_OS_PARTS; i++) {
		os[i].load(GFX_RES_OS + i, GFX_RES_OS_WIDTH, GFX_RES_OS_HEIGHT);
	}
	width = os[0].width;
	height = os[0].height;
}

void Library::release_pix() {
	int i;
	for (i=0; i < GFX_RES_OS_PARTS; i++) {
		os[i].release();
	}
}
