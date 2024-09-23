#ifndef GAME_H
#define GAME_H

#include "objects.h"

class Game {
	int efficiency;
public:
	unsigned state;
	unsigned long score;
	unsigned level, iteration;
	int grabbed;


	static const unsigned short scrwidth = 160;
	static const unsigned short scrheight = 160;

	static const int PLAYING = 1;
	static const int BETWEEN = 2;
	static const int END = 3;
	static const int SETUP = 4;
	static const int PAUSE = 5;

	static const int DEFAULTC = 0;		/* cursors */
	static const int DOWNC = -1;
	static const int BUCKETC = -2;

	static const int ENDLEVEL = -1;		/* Codes for updating score */
	static const int BILLPOINTS = 5;

	static const int EMPTY = -2;		/* Grabbed object */
	static const int BUCKET = -1;

	int RAND(int lb, int ub);
	int MAX(int x, int y);
	int MIN(int x, int y);
	int ABS(int x);
	int INTERSECT(int x1, int y1, int w1, int h1, int x2, int y2, int w2,
		int h2);

	void setup_level (unsigned int lev);
	void start(unsigned int lev);
	void draw_info();
	void update_score (int action);
	void warp_to_level (unsigned int lev);
	void button_press(int x, int y);
	void button_release(int x, int y);
	void refresh();
	void update();
	void init();
};

#endif
