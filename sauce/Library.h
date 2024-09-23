#ifndef LIBRARY_H
#define LIBRARY_H

#include "Picture.h"
#include "resources.h"

class Library {			/* global structure of all OSes */
public:
	static const int WINGDOWS = 0;	/* OS 0 is wingdows */
	static const int OFF = -1;	/* OS -1 means the computer is off */
	static const int PC = 6;	/* OS >= PC means the OS is a PC OS */
	int width, height;		/* size of OS picture*/
	Picture os[GFX_RES_OS_PARTS];		/* array of OS pictures*/
	void load_pix();
	void release_pix();
};

#endif
