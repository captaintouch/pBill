#include "Picture.h"
#include "objects.h"

void Picture::load(unsigned long *pics,unsigned long *masks,int w,int h) {
	pic=pics;
	mask=masks;
	width=w;
	height=h;
}
