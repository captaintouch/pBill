#ifndef UI_H
#define UI_H

#include "Picture.h"
#include <PalmOS.h>
class UI {
    int curx, cury;
    int updatedCursorX, updatedCursorY;
    Picture *cursorPic;
    WinHandle staticBackgroundBuffer;
    WinHandle backgroundBuffer;
    WinHandle screenBuffer;
    WinHandle mainWindow;

public:
    void set_cursor(int cursor);
    void update_cursor_position(int x, int y);
    Boolean UI::didMoveCursor();
    void draw_cursor();
    void clear();
    void reset();
    void refresh();
    void release();
    void pause(long millis);
    void draw(Picture picture, int x, int y);
    void draw_line(int x1, int y1, int x2, int y2);
    void draw_str(char *str, int x, int y);
    void show_msg(char *message);
    Boolean drawInBackground();
    void endDrawInBackground();
    void show_scorebox(int level, int score);

private:
    void writeBackgroundBufferToScreen();
    void deleteBackgroundBuffer();
    void activateBackgroundBuffer();
};

#endif
