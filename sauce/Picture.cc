#include "Picture.h"
#include "objects.h"
#include "drawhelper.h"

void Picture::load(int resId, int w, int h) {
	width=w;
	height=h;
	imageData = drawhelper_loadImage(resId);
}

void Picture::release() {
	drawhelper_releaseImageData(imageData);
}
