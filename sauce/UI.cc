#include "objects.h"
#include "drawhelper.h"
#include <Event.h>
#include <StringMgr.h>
#include <SystemMgr.h>

/*********************/
/* Graphics routines */
/*********************/

void UI::set_cursor(int cursor) {

    switch (cursor) {
    case game.BUCKETC:
        cursorPic = &bucket.picture;
        break;
    case game.DOWNC:
    case game.DEFAULTC:
        cursorPic = NONE;
        break;
    default:
        cursorPic = &(OS.os[cursor]);
    }
    if (cursorPic != NONE)
        curx = 999;
}

void UI::deleteBackgroundBuffer() {
    if (backgroundBuffer != NULL) {
        WinDeleteWindow(backgroundBuffer, false);
    }
}

void UI::activateBackgroundBuffer() {
    Err err = errNone;
    if (mainWindow == NULL) {
        mainWindow = WinGetDrawWindow();
    }

    deleteBackgroundBuffer();
    backgroundBuffer = WinCreateOffscreenWindow(160, 160, nativeFormat, &err);
    RectangleType lamerect;
    RctSetRectangle(&lamerect, 0, 0, 160, 160);
    WinCopyRectangle(staticBackgroundBuffer, backgroundBuffer, &lamerect, 0, 0, winPaint);
    WinSetDrawWindow(backgroundBuffer);
}

void UI::writeBackgroundBufferToScreen() {
    Err err = errNone;
    RectangleType lamerect;
    if (screenBuffer == NULL) {
        screenBuffer = WinCreateOffscreenWindow(160, 160, nativeFormat, &err);
    }
    RctSetRectangle(&lamerect, 0, 0, 160, 160);
    WinCopyRectangle(backgroundBuffer, screenBuffer, &lamerect, 0, 0, winPaint);
    WinSetDrawWindow(screenBuffer);
    draw_cursor();
    WinSetDrawWindow(backgroundBuffer);
    WinCopyRectangle(screenBuffer, mainWindow, &lamerect, 0, 0, winPaint);
}

void UI::update_cursor_position(int x, int y) {
    updatedCursorX = x - cursorPic->width / 2;
    updatedCursorY = y - cursorPic->height / 2;
}

void UI::draw_cursor() {
    if (!cursorPic)
        return;
    draw(*cursorPic, updatedCursorX, updatedCursorY);
    curx = updatedCursorX;
    cury = updatedCursorY;
}

void UI::clear() {
    activateBackgroundBuffer();
}

void UI::reset() {
    if (staticBackgroundBuffer != NULL) {
        WinDeleteWindow(staticBackgroundBuffer, false);
        staticBackgroundBuffer = NULL;
    }
}

Boolean UI::didMoveCursor() {
    return game.ABS(curx - updatedCursorX) + game.ABS(cury - updatedCursorY) > 10;
}

void UI::refresh() {
    writeBackgroundBufferToScreen();
}

Boolean UI::drawInBackground() {
    if (mainWindow == NULL) {
        mainWindow = WinGetDrawWindow();
    }
    if (staticBackgroundBuffer != NULL) {
        return false;
    }

    Err err = errNone;
    staticBackgroundBuffer = WinCreateOffscreenWindow(160, 160, nativeFormat, &err);
    WinSetDrawWindow(staticBackgroundBuffer);
    return true;
}

void UI::endDrawInBackground() {
    activateBackgroundBuffer();
    RectangleType lamerect;
    RctSetRectangle(&lamerect, 0, 0, 160, 160);
    WinCopyRectangle(staticBackgroundBuffer, backgroundBuffer, &lamerect, 0, 0, winPaint);
    WinSetDrawWindow(backgroundBuffer);
}

void UI::release() {
    if (staticBackgroundBuffer != NULL) {
        WinDeleteWindow(staticBackgroundBuffer, false);
        staticBackgroundBuffer = NULL;
    }
    if (backgroundBuffer != NULL) {
        WinDeleteWindow(backgroundBuffer, false);
        backgroundBuffer = NULL;
    }
    if (screenBuffer != NULL) {
        WinDeleteWindow(screenBuffer, false);
        screenBuffer = NULL;
    }
}

void UI::pause(long millis) {
    SysTaskDelay(millis * SysTicksPerSecond() / 1000);
}

void UI::draw(Picture pict, int x, int y) {
    drawhelper_drawPicture(pict, coordinate(x, y));
}

void UI::draw_line(int x1, int y1, int x2, int y2) {
    WinDrawLine(x1, y1, x2, y2);
}

void UI::draw_str(char *str, int x, int y) {
    drawhelper_drawText(str, coordinate(x, y));
}

#define MSG_WIDTH 90
#define MSG_HEIGHT 120
#define MSG_BORDER 5

void UI::show_msg(char *message) {
    int lines, wr, y, msgheight;
    RectangleType rc;
    /* determine no of lines */
    lines = 0;
    wr = 0;
    while (wr < StrLen(message)) {
        wr += FntWordWrap(&message[wr], MSG_WIDTH - 2 * MSG_BORDER);
        lines++;
    }
    msgheight = FntLineHeight() * lines + MSG_BORDER * 2;
    y = (160 - msgheight) / 2;

    /* draw white rectangle */
    drawhelper_applyForeColor(BSODBLUE);
    rc.topLeft.x = (160 - MSG_WIDTH) / 2;
    rc.extent.x = MSG_WIDTH;
    rc.topLeft.y = y;
    rc.extent.y = msgheight;
    WinDrawRectangle(&rc, 0);

    /* draw border */
    drawhelper_applyForeColor(CLOUDS);
    rc.extent.x = MSG_WIDTH;
    rc.extent.y = 1;
    WinDrawRectangle(&rc, 0);
    rc.extent.x = 1;
    rc.extent.y = msgheight;
    WinDrawRectangle(&rc, 0);

    drawhelper_applyForeColor(ASBESTOS);
    rc.extent.x = MSG_WIDTH;
    rc.topLeft.y = y + msgheight;
    rc.extent.y = 1;
    WinDrawRectangle(&rc, 0);
    rc.topLeft.x = (160 + MSG_WIDTH) / 2;
    rc.extent.x = 1;
    rc.topLeft.y = y;
    rc.extent.y = msgheight + 1;
    WinDrawRectangle(&rc, 0);

    drawhelper_applyForeColor(ASBESTOS);

    /* draw lines */
    wr = 0;
    y += MSG_BORDER;
    while (wr < StrLen(message)) {
        if (message[wr] == '\n')
            wr++;
        lines = FntWordWrap(&message[wr], MSG_WIDTH - 2 * MSG_BORDER);
        if (message[wr + lines - 1] == '\n')
            lines--;
        drawhelper_applyBackgroundColor(BSODBLUE);
        drawhelper_applyTextColor(CLOUDS);
        drawhelper_drawTextWithLength(&message[wr], lines, coordinate(80 - (FntLineWidth(&message[wr], lines) / 2), y));
        wr += lines;
        y += FntLineHeight();
    }

    refresh();
}

/******************/
/* Other routines */
/******************/

void UI::show_scorebox(int level, int score) {
    char msg[40];
    StrPrintF(msg, "Level %d completed\nScore:%d.", level, score);
    show_msg(msg);
}