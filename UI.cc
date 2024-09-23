#include <SystemMgr.h>
#include <StringMgr.h>
#include <Event.h>
#include "objects.h"
#include "win2.h"

/*********************/
/* Graphics routines */
/*********************/

void UI::set_cursor(int cursor) {

	switch (cursor) {
	case game.BUCKETC:
		cursorPic=&bucket.picture;
		break;
	case game.DOWNC:
	case game.DEFAULTC:
		cursorPic=NONE;
		break;
	default:
		cursorPic=&(OS.os[cursor]);
	}
	if (cursorPic!=NONE)
	  curx=999;
}

void UI::draw_cursor(int x,int y)
{
  if (!cursorPic) return;
	x-=cursorPic->width/2;
	y-=cursorPic->height/2;
	if (curx!=999)
  	Win2BlatAreaToScreen(curx,cury,cursorPic->width,cursorPic->height);
	Win2DrawToBuffer(0);
	draw(*cursorPic,x,y);
	Win2DrawToBuffer(1);
	curx=x;
	cury=y;
}

void UI::clear() {
  Win2ClearScreen();
}

void UI::refresh() {
  Win2BlatBufferToScreen();
	if (cursorPic&&curx!=999) {
	  Win2DrawToBuffer(0);
		draw(*cursorPic,curx,cury);
		Win2DrawToBuffer(1);
	}
}

void UI::pause(long millis) {
  SysTaskDelay(millis*SysTicksPerSecond()/1000);
}

void UI::draw (Picture pict, int x, int y) {
  if (pict.mask!=NONE)
	  Win2DrawMaskedImage(pict.pic,pict.mask,x,y,pict.width,pict.height);
	else
	  Win2DrawImage(pict.pic,x,y,pict.width,pict.height);
}

void UI::draw_line(int x1, int y1, int x2, int y2) {
  Win2DrawLine(x1,y1,x2,y2);
}

void UI::draw_str(char *str, int x, int y) {
  Win2DrawChars(str,StrLen(str),x,y);
}

#define MSG_WIDTH  90
#define MSG_HEIGHT 120
#define MSG_BORDER 5

void UI::show_msg(char *message) {
  int lines,wr,y,msgheight;
  RectangleType rc;

  /* determine no of lines */
  lines=0; wr=0;
  while(wr<StrLen(message)) {
    wr+=FntWordWrap(&message[wr], MSG_WIDTH-2*MSG_BORDER);
    lines++;
  }
  msgheight=FntLineHeight()*lines+MSG_BORDER*2;
  y=(160-msgheight)/2;

  /* draw white rectangle */
  Win2SetColor(clrLtGrey);
  rc.topLeft.x=(160-MSG_WIDTH)/2;  rc.extent.x=MSG_WIDTH;
  rc.topLeft.y=y; rc.extent.y=msgheight;
  Win2FillRect(&rc,0,0);

  /* draw border */
  Win2SetColor(clrWhite);
  rc.extent.x=MSG_WIDTH; rc.extent.y=1;
  Win2FillRect(&rc,0,3);
  rc.extent.x=1;         rc.extent.y=msgheight;
  Win2FillRect(&rc,0,3);

  Win2SetColor(clrBlack);
  rc.extent.x=MSG_WIDTH; rc.topLeft.y=y+msgheight; rc.extent.y=1;
  Win2FillRect(&rc,0,3);
  rc.topLeft.x=(160+MSG_WIDTH)/2;  rc.extent.x=1;
  rc.topLeft.y=y; rc.extent.y=msgheight+1;
  Win2FillRect(&rc,0,3);

  Win2SetColor(clrBlack);

  /* draw lines */
  wr=0;
  y+=MSG_BORDER;
  while(wr<StrLen(message)) {
    if (message[wr]=='\n') wr++;
    lines=FntWordWrap(&message[wr], MSG_WIDTH-2*MSG_BORDER);
    if (message[wr+lines-1]=='\n') lines--;
    Win2DrawChars(&message[wr],lines, 80-(FntLineWidth(&message[wr],lines)/2), y);
    wr+=lines;
    y+=FntLineHeight();
  }
}

/******************/
/* Other routines */
/******************/

void UI::show_scorebox(int level, int score) {
  char msg[40];
	StrPrintF(msg,"Level %d completed\nScore:%d.",level,score);
	show_msg(msg);
}