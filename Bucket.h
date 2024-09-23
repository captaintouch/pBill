#ifndef BUCKET_H
#define BUCKET_H

#include "Picture.h"

class Bucket {
public:
	Picture picture;
	int width, height;
	void draw();
	int clicked(int x, int y);
	void load_pix();
};

#endif
