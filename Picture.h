#ifndef X11_PICTURE_H
#define X11_PICTURE_H

class Picture {
public:
	int width, height;
	unsigned long *pic;
	unsigned long *mask;
	void load(unsigned long *pics,unsigned long *masks, int width,int height);
};

#endif
