#ifndef UI_H
#define UI_H

#include "Picture.h"

class UI {
	int curx,cury;
	Picture *cursorPic;

public:
	void set_cursor (int cursor);
  void draw_cursor(int x,int y);
	void clear();
	void refresh();
	void pause(long millis);
	void draw (Picture picture, int x, int y);
	void draw_line (int x1, int y1, int x2, int y2);
	void draw_str (char *str, int x, int y);
  void show_msg(char *message);

	void show_scorebox(int level, int score);
};

#endif
