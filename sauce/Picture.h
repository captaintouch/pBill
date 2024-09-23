#ifndef X11_PICTURE_H
#define X11_PICTURE_H
#include <PalmOS.h>

typedef struct ImageData {
    MemHandle resource;
    BitmapPtr bitmapPtr;
} ImageData;

class Picture {
public:
	int width, height;
    ImageData imageData;
	void load(int resourceId, int width, int height);
    void release();
};

#endif