/*---------------------------------------------------------------------
| win2.h
|
|	WIN2 2bpp greyscale replacement for Win* Palm routines
|	To use, replace all calls to Win* palm APIs with Win2*
|
|	See readme.txt for usage information
|
|	(c) 1997-1998 ScumbySoft - wesc@ricochet.net
-----------------------------------------------------------------------*/

#include "win2opts.h"

#ifdef __cplusplus
extern "C" {
#endif
/*---------------------------------------------------------------------
| Win2 Colors
-----------------------------------------------------------------------*/
enum win2Color
	{
	clrWhite,
	clrLtGrey,
	clrDkGrey,
	clrBlack
	};

typedef enum win2Color Win2Color;

/*-----------------------------------------------------------------------------
|	Win2SetGreyscale
|
|		Switches Pilots display into greyscale mode
|	Note, this allocates memory in a variety of ways.  See win2opts.h
|	for more information
|
|	Returns: 0 if successful, nonzero if not
-------------------------------------------------------wesc@ricochet.net-----*/
int Win2SetGreyscale(int buffered);
/*-----------------------------------------------------------------------------
|	Win2SetMono
|
|		Switches back to mono mode.
|
|	Returns: 0 if successful, nonzero if not
-------------------------------------------------------wesc@ricochet.net-----*/
int  Win2SetMono();
/*-----------------------------------------------------------------------------
|	Win2SetColor
|
|		Sets the current drawing color.  All Fill, Line and Text
|	operations will use this color
|
|	Arguments:
|		Win2Color clr
|
|	Returns: old color
-------------------------------------------------------wesc@ricochet.net-----*/
Win2Color Win2SetColor(Win2Color clr);
/*-----------------------------------------------------------------------------
|	Win2SetBackgroundColor
|
|		Sets the background color for Win2DrawChars
|
|	Arguments:
|		Win2Color clr
|
|	Returns: old color
-------------------------------------------------------wesc@ricochet.net-----*/
Win2Color Win2SetBackgroundColor(Win2Color clr);

/*-----------------------------------------------------------------------------
|	Win2CopyMonoToGrey
|
|		Copies the contents of the monochrome screen to the grey screen
|
|	Arguments:
|		Boolean fOrMode:  if true then or's mono bits with dest grey
-------------------------------------------------------wesc@ricochet.net-----*/
void Win2CopyMonoToGrey(Boolean fOrMode);
/*-----------------------------------------------------------------------------
|	Win2PreFilterEvent
|
|		Helper -- put in your main event loop to handle common cases
|	where the pilot should be put back into mono mode
|
|	Arguments:
|		const EventType *pevt
|
|	Returns: true if event is processed
-------------------------------------------------------wesc@ricochet.net-----*/
Boolean Win2PreFilterEvent(const EventType *pevt);


/*-----------------------------------------------------------------------------
|	Win2DrawLine
|
|		Draws a line from x1,y1 to x2,y2
|
|	Arguments:
|		SWord x1
|		SWord y1
|		SWord x2
|		SWord y2
-------------------------------------------------------wesc@ricochet.net-----*/
void Win2DrawLine(SWord x1, SWord y1, SWord x2, SWord y2);
/*-----------------------------------------------------------------------------
|	Win2EraseRectangle
|
|		Erases a rectangle on the screen (makes it clrWhite)
|	Note that round-rects are not supported
|
|	Arguments:
|		RectanglePtr r
|		Word cornerDiam: ignored!
-------------------------------------------------------wesc@ricochet.net-----*/
void Win2EraseRectangle(RectanglePtr r, Word cornerDiam);
/*-----------------------------------------------------------------------------
|	Win2FillRectangle
|
|		Fills a rectangle on the screen with the current color
|	Note that round-rects are not supported
|
|	Arguments:
|		RectanglePtr r
|		Word cornerDiam: ignored!
-------------------------------------------------------wesc@ricochet.net-----*/
void Win2FillRectangle(RectanglePtr r, Word cornerDiam);
/*-----------------------------------------------------------------------------
|	Win2DrawChars
|
|		Draws chars on the screen.  Uses current color and background color
|	Note that Underline mode is not supported!
|
|	Arguments:
|		CharPtr chars
|		Word len
|		SWord x
|		SWord y
-------------------------------------------------------wesc@ricochet.net-----*/
void Win2DrawChars(CharPtr chars, Word len, SWord x, SWord y);

/*-----------------------------------------------------------------------------
|	Win2DrawBitmap
|
|		Draws a bitmap on the screen.  It must be a 2bpp bitmap authored
|	via PilRC.  Unpredictable results will occur if you pass in a 1bpp
|	bitmap.
|
|	Arguments:
|		BitmapPtr bitmapP
|		SWord x
|		SWord y
-------------------------------------------------------wesc@ricochet.net-----*/
void Win2DrawBitmap(BitmapPtr bitmapP, SWord x, SWord y);

/*-----------------------------------------------------------------------------
|	Aliased routines.  Win2 routines that just define to the old
|	Win* routines
-------------------------------------------------------wesc@ricochet.net-----*/
#define Win2SetActiveWindow(winHandle) WinSetActiveWindow(winHandle)
#define Win2SetDrawWindow(winHandle)   WinSetDrawWindow(winHandle)
#define Win2GetDrawWindow(void)        WinGetDrawWindow(void)
#define Win2GetActiveWindow(void)      WinGetActiveWindow(void)
#define Win2GetFirstWindow()                 WinGetFirstWindow()
#define Win2EnableWindow(winHandle)          WinEnableWindow(winHandle)
#define Win2DisableWindow(winHandle)         WinDisableWindow(winHandle)
#define Win2GetWindowFrameRect(winHandle, r) WinGetWindowFrameRect(winHandle, r)
#define Win2GetDisplayExtent(extentX, extentY) WinGetDisplayExtent(extentX,  extentY)
#define Win2GetWindowBounds(r) WinGetWindowBounds(r)
#define Win2SetWindowBounds(winHandle, r)       WinSetWindowBounds(winHandle, r)
#define Win2GetWindowExtent(extentX,   extentY) WinGetWindowExtent(extentX,   extentY)
#define Win2DisplayToWindowPt(extentX, extentY) WinDisplayToWindowPt(extentX, extentY)
#define Win2WindowToDisplayPt(extentX, extentY) WinWindowToDisplayPt(extentX, extentY)
#define Win2GetClip(r)       WinGetClip(r)
#define Win2SetClip(r)       WinSetClip(r)
#define Win2ResetClip()      WinResetClip()
#define Win2ClipRectangle(r) WinClipRectangle()
#define Win2Modal(winHandle) WinModal(winHandle)
#define Win2GetFramesRectangle(frame, r, obscuredRect) WinGetFramesRectangle(frame, r, obscuredRect)
#define Win2SetUnderlineMode(mode) WinSetUnderlineMode(mode)

#define Win2GetPattern(pat) WinGetPattern(pat)
#define Win2SetPattern(pat) WinGetPattern(pat)

/*-----------------------------------------------------------------------------
|	NYI routines.  The following are not currently implemented in Win2
-------------------------------------------------------wesc@ricochet.net-----*/
//NYI WinHandle Win2CreateWindow (RectanglePtr bounds, FrameType frame, Boolean modal, Boolean focusable, WordPtr error)
//NYI WinHandle Win2CreateOffscreenWindow (SWord width, SWord height, WindowFormatType format, WordPtr error)
//NYI void Win2DeleteWindow (WinHandle winHandle, Boolean eraseIt)
//NYI void Win2InitializeWindow (WinHandle winHandle)
//NYI void Win2AddWindow (WinHandle winHandle)
//NYI void Win2RemoveWindow (WinHandle winHandle)
//NYI WinHandle Win2GetDisplayWindow (void)
//NYI void Win2DrawWindowFrame (void)
//NYI void Win2EraseWindow (void)
//NYI WinHandle Win2SaveBits (RectanglePtr source, WordPtr error)
//NYI void Win2RestoreBits (WinHandle winHandle, SWord destX, SWord destY)
//NYI void Win2CopyRectangle (WinHandle srcWin2, WinHandle dstWin2,
//NYI void Win2ScrollRectangle(RectanglePtr r, enum directions direction, SWord distance, RectanglePtr vacated)

//NYI void Win2DrawGrayLine (SWord x1, SWord y1, SWord x2, SWord y2)
//NYI void Win2EraseLine (SWord x1, SWord y1, SWord x2, SWord y2)
//NYI void Win2InvertLine (SWord x1, SWord y1, SWord x2, SWord y2)
//NYI void Win2FillLine (SWord x1, SWord y1, SWord x2, SWord y2)
//NYI void Win2DrawRectangle (RectanglePtr r, Word cornerDiam);
//NYI void Win2InvertRectangle (RectanglePtr r, Word cornerDiam)
//NYI void Win2DrawRectangleFrame (FrameType frame, RectanglePtr r)
//NYI void Win2DrawGrayRectangleFrame (FrameType frame, RectanglePtr r)
//NYI void Win2EraseRectangleFrame (FrameType frame, RectanglePtr r)
//NYI void Win2InvertRectangleFrame (FrameType frame, RectanglePtr r)
//NYI void Win2DrawInvertedChars (CharPtr chars, Word len, SWord x, SWord y)
//NYI void Win2EraseChars (CharPtr chars, Word len, SWord x, SWord y)
//NYI void Win2InvertChars (CharPtr chars, Word len, SWord x, SWord y)

int Win2FillRect(RectangleType *prc, int dxyCornder, int clr);
int Win2IsGreyscale();
void Win2DrawToBuffer(int drawToBuffer);
void Win2BlatAreaToScreen(int x,int y,int width,int height);
void Win2BlatBufferToScreen();
void Win2ClearScreen();
void Win2DrawImage(unsigned long *pics,int x,int y,int width,int height);
void Win2DrawMaskedImage(unsigned long *pics,unsigned long *masks,int x,int y,int width,int height);

#ifdef __cplusplus
}
#endif
