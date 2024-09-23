/*---------------------------------------------------------------------
| win2.c
|
|	WIN2 2bpp greyscale replacement for Win* Palm routines
|	To use, replace all calls to Win* palm APIs with Win2*
|
|	See readme.txt for usage information
|
|	(c) 1997-1998 ScumbySoft - wesc@ricochet.net
-----------------------------------------------------------------------*/

#define NON_PORTABLE
#include <Common.h>
#include <System/SysAll.h>
#include <UI/UIAll.h>
#include <System/Memoryprv.h>
#include "win2.h"


// Define auxillary routines dependent on win2opts settings
#if defined(WIN2DRAWCHARS)
#define WIN2DRAWBITMAPEX
#endif

#if defined(WIN2DRAWBITMAP)
#define WIN2DRAWBITMAPEX
#endif

int Win2DrawBitmapEx(Byte *pbBits, short xSrc, short ySrc, short xDest, short yDest, short dxDest, short dyDest, short rowBytes, Boolean fDrawChar);

/* defines */
#define VPW ((unsigned char *)0xFFFFFA05)
#define LSSA ((unsigned long *)0xFFFFFA00)
#define PICF ((unsigned char *)0xFFFFFA20)
#define CKCON ((unsigned char *)0xFFFFFA27)
#define LBAR ((unsigned char *)0xFFFFFA29)
#define FRCM ((unsigned char *)0xFFFFFA31)
#define LGPMR ((unsigned short *)0xFFFFFA32)
#define REVERSE(D)				(D)

/* vars */

void _Win2SetShades(short sh0,short sh1,short sh2,short sh3);
void _Win2SwitchDisplayModeGrey();
void _Win2SwitchDisplayModeBW();



typedef struct
	{
	Win2Color clr;
	Win2Color clrBackground;
//	WinHandle winHandle;
  short buffered;
	short drawingToBuffer;
  Byte *pbGreyRealScreenBase;
	Byte *pbGreyScreenBase;
	Byte *pbMonoScreenBase;
	short fGreyScale;
	} Win2Type;


Win2Type vwin2 =
	{
	clrBlack,	// clr
	clrWhite,	// clrBackground
	};

#ifdef DWORDBLT
// this requires source to be dword padded.  I don't want to do that.
#define cbppGrey 2		// bits per pixel grey
#define cppbuGrey 16		// pixels per BltUnit grey
#define cbpbu 32			// bits per BltUnit
#define shppbuGrey 4		// shift count pixels->BltUnit
#define buAllOnes 0xFFFFFFFF
#define BltUnit DWord
#else
#define cbppGrey 2		// bits per pixel grey
#define cppbuGrey 8		// pixels per BltUnit grey
#define cbpbu 16			// bits per BltUnit
#define shppbuGrey 2		// shift count pixels->BltUnit
#define buAllOnes 0xFFFF
#define BltUnit Word
#endif
#define dxScreen 160
#define dyScreen 160
#define cbGreyScreen (dxScreen*cbppGrey/8*dyScreen)
#define MonoBToBltUnit(b) ((mpNibbleGreyByte[(b)>>4]<<8)|mpNibbleGreyByte[b&0xf])
#define WriteMask(buDest,buSrc,buMask)  (buDest=((buDest)&~(buMask))|((buSrc)&(buMask)))

BltUnit mpclrbuRep[] =
	{
#ifdef DWORDBLT
	0x00000000,
	0x55555555,
	0xAAAAAAAA,
	0xFFFFFFFF,
#else
	0x0000,
	0x5555,
	0xAAAA,
	0xFFFF
#endif
	};




#ifdef WIN2ALLOCLOCKEDCHUNK
/*-----------------------------------------------------------------------------
|	PvAllocLockedChunk
|
|		Allocates a locked chunk out of storage RAM (not dynamic).
|
|	Note that you must call MemSemaphoreReserve(1) to gain write-access to the chunk
|	(call MemSemaphoreRelease(1) to restore protection)
|
|	Free memory allocated by PvAllocLockedChunk via MemChunkFree
|
|	If no allocation succeeds on the first pass then it makes a second
|	pass compacting the heaps before allocating.
|
|
|	Arguments:
|		int cb:  number of bytes to allocate
|
|	Returns: pointer to bytes allocated
|
|	Keywords:
|
-------------------------------------------------------------WESC------------*/
VoidPtr PvAllocLockedChunk(int cb)
	{
	int ccard;
	int icard;
	int ipass;
	VoidPtr pv;

	ccard = MemNumCards();
	for (ipass = 0; ipass < 2; ipass++)
		{
		for (icard = 0; icard < ccard; icard++)
			{
			// don't bother if this card is all ROM
			if (MemNumRAMHeaps(icard) > 0)
				{
				int idHeap;
				int cidHeap;

				cidHeap = MemNumHeaps(icard);
				for (idHeap = 0; idHeap < cidHeap; idHeap++)
					{
					// We don't want ROM or Dynamic heaps
					if (!(MemHeapFlags(idHeap) & memHeapFlagReadOnly) && !MemHeapDynamic(idHeap))
						{
						//UInt cbFree;
						//UInt cbMax;

						// try two -- let's try compacting the heap
						if (ipass == 1)
							MemHeapCompact(idHeap);
						// MemHeapFreeBytes(idHeap, &cbFree, &cbMax);
						pv = MemChunkNew(idHeap, cb, memNewChunkFlagNonMovable);

						if (pv != NULL)
							{
							return pv;
							}
						}
					}
				}
			}
		}
	// we couldn't find any heaps so bail
	return NULL;
	}

int _Win2FAllocScreen(int buffered)
	{
  vwin2.pbGreyScreenBase = PvAllocLockedChunk(cbGreyScreen);
  if (buffered) {
    vwin2.pbGreyRealScreenBase = PvAllocLockedChunk(cbGreyScreen);
    if (vwin2.pbGreyRealScreenBase == NULL)
      buffered=0;
  }
  vwin2.buffered=buffered;
	return vwin2.pbGreyScreenBase != NULL;
	}
void _Win2ScreenAccess(Boolean fAccess)
	{
	if (fAccess)
		MemSemaphoreReserve(1);
	else
		MemSemaphoreRelease(1);
	}

void _Win2FreeScreen()
	{
	if (vwin2.pbGreyScreenBase != NULL)
		{
		MemChunkFree(vwin2.pbGreyScreenBase);
    if (buffered)
      MemChunkFree(vwin2.pbGreyRealScreenBase);
		vwin2.pbgreyScreen = NULL;
		}
	}
#endif // WIN2ALLOCLOCKEDCHUNK

#ifdef WIN2ALLOCDATABASE
char szScreen[] = "GreyScreen";
VoidHand vhGreyScreen;
VoidHand vhRealGreyScreen;
DmOpenRef vdmorGreyScreen;
int _Win2DoAllocScreen(int buffered)
	{
	LocalID id;
	Err err;
	ULong cRecords;
	ULong cb;
	ULong cbData;

	id = DmFindDatabase(0, szScreen);
	if (id == 0)
		{
		// BUG!!! need app id here
		err = DmCreateDatabase(0, szScreen, 'Grey', 'Grey', 0);
		if (err != 0)
			return 0;
		id = DmFindDatabase(0, szScreen);
		if (id == 0)
			return 0;
		}
	vdmorGreyScreen = DmOpenDatabase(0, id, dmModeReadWrite);
	if (vdmorGreyScreen == 0)
		return 0;
	DmDatabaseSize(0, id, &cRecords, &cb, &cbData);
	if (cRecords > 0)
  {
		vhGreyScreen = DmGetRecord(vdmorGreyScreen, 0);
    if (buffered)
      vhRealGreyScreen = DmGetRecord(vdmorGreyScreen, 1);
  }
	if (vhGreyScreen == NULL)
		{
		UInt i;
		i = 0;
		vhGreyScreen = DmNewRecord(vdmorGreyScreen, &i, cbGreyScreen);
    if (buffered)
      vhRealGreyScreen = DmNewRecord(vdmorGreyScreen, &1, cbGreyScreen);
		}
	if (vhGreyScreen == NULL)
		return 0;
  if (buffered&&vhRealGreyScreen==NULL)
    buffered=0;
	vwin2.pbGreyScreenBase = MemHandleLock(vhGreyScreen);
  if (buffered) {
    vwin2.pbGreyRealScreenBase = MemHandleLock(vhRealGreyScreen);
    if (vwin2.pbGreyRealScreenBase==NULL)
      buffered=0;
  }
  vwin2.buffered=buffered;
	return vwin2.pbGreyScreenBase != NULL;
	}



void _Win2ScreenAccess(Boolean fAccess)
	{
	if (fAccess)
		MemSemaphoreReserve(1);
	else
		MemSemaphoreRelease(1);
	}

void _Win2FreeScreen()
	{
	LocalID id;

	if (vhGreyScreen != NULL)
		{
		MemHandleUnlock(vhGreyScreen);
    MemHandleUnlock(vhRealGreyScreen);
		DmReleaseRecord(vdmorGreyScreen, 0, 0);
    DmReleadeRecord(vdmorGreyScreen, 1, 0);
		}
	if (vdmorGreyScreen != NULL)
		DmCloseDatabase(vdmorGreyScreen);
	id = DmFindDatabase(0, szScreen);
	if (id != 0)
		DmDeleteDatabase(0, id);
	vhGreyScreen = NULL;
  vhRealGreyScreen = NULL;
	vdmorGreyScreen = NULL;
	vwin2.pbGreyScreenBase = NULL;
  vwin2.pbGreyRealScreenBase = NULL;
	}

int _Win2FAllocScreen()
	{
	if (!_Win2DoAllocScreen())
		{
		_Win2FreeScreen();
		return 0;
		}
	return 1;
	}
#endif // WIN2ALLOCDATABASE

#ifdef WIN2ALLOCPTR
int _Win2FAllocScreen(int buffered)
	{
  vwin2.pbGreyScreenBase = (Byte *)MemPtrNew(cbGreyScreen);
  if (buffered) {
    vwin2.pbGreyRealScreenBase = (Byte *)MemPtrNew(cbGreyScreen);
    if (vwin2.pbGreyRealScreenBase==NULL)
      buffered=0;
  }
  vwin2.buffered=buffered;
	return vwin2.pbGreyScreenBase != NULL;
	}
void _Win2FreeScreen()
	{
	if (vwin2.pbGreyScreenBase != NULL)
		{
		MemPtrFree(vwin2.pbGreyScreenBase);
    if (vwin2.buffered)
      MemPtrFree(vwin2.pbGreyRealScreenBase);
		vwin2.pbGreyScreenBase = NULL;
    vwin2.pbGreyRealScreenBase = NULL;
		}
	}

#define _Win2ScreenAccess(f)
#endif // WIN2ALLOCPTR

/*-----------------------------------------------------------------------------
|	Win2SetGreyscale
|
|		Switches Pilots display into greyscale mode
|	Note, this allocates memory in a variety of ways.  See win2opts.h
|	for more information
|
|	Returns: 0 if successful, nonzero if not
-------------------------------------------------------wesc@ricochet.net-----*/
int Win2SetGreyscale(int buffered)
	{
	if (vwin2.fGreyScale==0)	/* only chage if req. */
		{
		if (!_Win2FAllocScreen(buffered))
			return(1);	/* cant get mem for screen */

		vwin2.pbMonoScreenBase=(Byte*)*LSSA;

//		Win2CopyMonoToGrey(0);
		_Win2ScreenAccess(1);
		MemSet(vwin2.pbGreyScreenBase, cbGreyScreen, 0);
    if (vwin2.buffered)
      MemSet(vwin2.pbGreyRealScreenBase, cbGreyScreen, 0);
		_Win2ScreenAccess(0);
		_Win2SwitchDisplayModeGrey();

    if (vwin2.buffered) {
			vwin2.drawingToBuffer=1;
      *LSSA=(long)vwin2.pbGreyRealScreenBase;
	  }
    else
  		*LSSA=(long)vwin2.pbGreyScreenBase;
		vwin2.fGreyScale=1;
		}
	return(0);
	}

/*-----------------------------------------------------------------------------
|	Win2SetMono
|
|		Switches back to mono mode.
|
|	Returns: 0 if successful, nonzero if not
-------------------------------------------------------wesc@ricochet.net-----*/
int Win2SetMono()
	{
	if (vwin2.fGreyScale!=1)
		return 1;	/* already mono */

	*LSSA=(long)vwin2.pbMonoScreenBase;
	_Win2SwitchDisplayModeBW();

	_Win2FreeScreen();
	vwin2.fGreyScale=0;
	return 0;
	}

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
Boolean Win2PreFilterEvent(const EventType *pevt)
	{
	if (pevt->eType == keyDownEvent && (pevt->data.keyDown.modifiers & commandKeyMask))
		{
		switch (pevt->data.keyDown.chr)
			{
		default:
			Win2SetMono();
			break;
		case keyboardChr:
		case	graffitiReferenceChr:		// popup the Graffiti reference
		case	keyboardAlphaChr:		// popup the keyboard in alpha mode
		case	keyboardNumericChr:		// popup the keyboard in number mode
			return 1;	 // eat, don't switch to mono
		case hardPowerChr:
		case backlightChr:
			// don't switch to mono, but don't eat either
			break;
			}
		}
	return 0;
	}

const unsigned char mpNibbleGreyByte[]={0x00,0x03,0x0c,0x0f,
					0x30,0x33,0x3c,0x3f,
					0xc0,0xc3,0xcc,0xcf,
					0xf0,0xf3,0xfc,0xff};


/*-----------------------------------------------------------------------------
|	Win2CopyMonoToGrey
|
|		Copies the contents of the monochrome screen to the grey screen
|
|	Arguments:
|		Boolean fOrMode:  if true then or's mono bits with dest grey
-------------------------------------------------------wesc@ricochet.net-----*/
void Win2CopyMonoToGrey(Boolean fOrMode)
	{
	BltUnit *pbuDest;
	Byte *pbSrc;
	Byte *pbMac;

	_Win2ScreenAccess(1);
	pbuDest = (BltUnit *)vwin2.pbGreyScreenBase;
	pbSrc = vwin2.pbMonoScreenBase;
	pbMac = pbSrc+dxScreen/8*dyScreen;
	if (fOrMode)
		while(pbSrc < pbMac)
			{
			*pbuDest++ |= MonoBToBltUnit(*pbSrc);
			pbSrc++;
			}
	else
		while(pbSrc < pbMac)
			{
			*pbuDest++ = MonoBToBltUnit(*pbSrc);
			pbSrc++;
			}
	_Win2ScreenAccess(0);
	}



void _Win2SwitchDisplayModeGrey()
	{
	*CKCON=*CKCON & 0x7F;	/* display off*/

	/*virtual page width now 40 bytes (160 pixels)*/
	*VPW=20;
	*PICF=*PICF | 0x01; /*switch to grayscale mode*/
	*LBAR=20; /*line buffer now 40 bytes*/

	/*register to control grayscale pixel oscillations*/
	*FRCM=0xB9;

	/*let the LCD get to a 2 new frames (40ms delay) */
	SysTaskDelay(4);

	/*switch LCD back on */
	*CKCON=*CKCON | 0x80;

	_Win2SetShades(0, 3, 4, 7);	/* set palette */
	}


void _Win2SwitchDisplayModeBW()
	{
	//switch off LCD update temporarily
	*CKCON=*CKCON & 0x7F;

	//set new display starting address
	/*		*SSA=(ULong)*displayaddr;*/
	//virtual page width now 20 bytes (160 pixels)
	*VPW=10;
	*PICF=*PICF & 0xFE; //switch to black and white mode
	*LBAR=10; // line buffer now 20 bytes

	//let the LCD get to a new frame (20ms delay)
	SysTaskDelay(4);

	//switch LCD back on in new mode
	*CKCON=*CKCON | 0x80;
	//return original display address for switch back
	/*		*displayaddr=oldstart;*/
	}


void _Win2SetShades(short sh0,short sh1,short sh2,short sh3)
	{
	*LGPMR=(sh0<<8)+(sh1<<12)+sh2+(sh3<<4);
	}



int _Win2FClipRect(RectangleType *prc)
	{
	RectangleType rcClip;
	RectangleType rcBlt;

	WinGetClip(&rcClip);
	RctGetIntersection (&rcClip, prc, &rcBlt);
	*prc = rcBlt;
	return rcBlt.extent.x > 0 && rcBlt.extent.y > 0;
	}

#if defined(WIN2DRAWBITMAP)
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
void Win2DrawBitmap(BitmapPtr bitmapP, SWord x, SWord y)
	{
	RectangleType rcDest;

	rcDest.topLeft.x = x;
	rcDest.topLeft.y = y;
	rcDest.extent.x = bitmapP->width;
	rcDest.extent.y = bitmapP->height;
	if (_Win2FClipRect(&rcDest))
		Win2DrawBitmapEx((Byte *)(bitmapP+1), 0+rcDest.topLeft.x-x, rcDest.topLeft.y-y, rcDest.topLeft.x, rcDest.topLeft.y, rcDest.extent.x, rcDest.extent.y, bitmapP->rowBytes, 0);
	}
#endif // WIN2DRAWBITMAP

#if defined(WIN2DRAWBITMAPEX)
int Win2DrawBitmapEx(Byte *pbBits, short xSrc, short ySrc, short xDest, short yDest, short dxDest, short dyDest, short rowBytes, Boolean fDrawChar)
	{
	int xDestRight;
	int yDestBot;
	BltUnit *pbuBaseDest;
	BltUnit *pbuBaseSrc;
	int yDestCur;
	int xDestCur;
	int cPelsLeft;
	BltUnit  buMaskLeft;
	int shSrc;
	int cPelsRight;
	BltUnit buMaskRight;
	BltUnit buClrMask;
	BltUnit buClrMaskBackground;
	BltUnit rgbuSrc[8];	// handles up to 64 pixel wide font...otta be enough
	int dxSrcTranslate;
	int   cPelsNextSrc;

	if (vwin2.pbGreyScreenBase == 0)
		return 0;
	_Win2ScreenAccess(1);
	WinWindowToDisplayPt(&xDest, &yDest);

	shSrc = ((xSrc + cppbuGrey - (xDest % cppbuGrey)) % cppbuGrey) * cbppGrey;
	pbuBaseDest = ((BltUnit *)vwin2.pbGreyScreenBase)+(dxScreen/cppbuGrey)*yDest + xDest/(cppbuGrey);
	xDestRight = xDest + dxDest;	// pbm->width;
	yDestBot = yDest + dyDest;	// pbm->height;


	if (fDrawChar)
		{
		dxSrcTranslate = (xSrc%8+dxDest+7)/8;
		// get mask for current color
		buClrMask = mpclrbuRep[vwin2.clr];
		buClrMaskBackground = mpclrbuRep[vwin2.clrBackground];
		pbuBaseSrc = (BltUnit *) (pbBits + rowBytes*ySrc + xSrc/8);
		}
	else
		pbuBaseSrc = (BltUnit *)(pbBits);


	// precompute masks and stuff
	cPelsLeft = 0;
	if(xDest % cppbuGrey)
		{
		int dwRightShift;

		cPelsLeft = cppbuGrey - (xDest % cppbuGrey);
		dwRightShift = (xDest * cbppGrey) % cbpbu;
		buMaskLeft = buAllOnes >> dwRightShift;
		if(xDest + cPelsLeft > xDestRight)
			{
			int cPelsRightDontTouch;

			cPelsRightDontTouch = xDest + cPelsLeft - xDestRight;
			buMaskLeft &= buAllOnes << (cPelsRightDontTouch * cbppGrey);
			cPelsLeft -= cPelsRightDontTouch;
			}
		cPelsNextSrc = cPelsLeft - (cppbuGrey - (xSrc % cppbuGrey));
		}
	// precompute right mask
	if ((cPelsRight = (xDestRight % cppbuGrey)))
		{
		buMaskRight = buAllOnes << (cppbuGrey - cPelsRight) * cbppGrey;
		}


	for(yDestCur = yDest; yDestCur < yDestBot; yDestCur++)
		{
		BltUnit  *pbuDest;
		BltUnit  *pbuSrc;
		BltUnit  buSrc;
		BltUnit  buSrcData, buSrcNext;

		pbuDest = pbuBaseDest;
		xDestCur = xDest;
		if (fDrawChar)
			{
			Byte *pbSrcT;
			int x;

			pbSrcT = (Byte *)pbuBaseSrc;
			for (x = 0; x < dxSrcTranslate; x++)
				{
				BltUnit bu;

				bu  = MonoBToBltUnit(*pbSrcT);
				// make all 1 bits be current color
				// make all 0 bits be current bkgnd color
				bu = (bu & buClrMask) | ((~bu) & buClrMaskBackground);
				rgbuSrc[x] = bu;
				pbSrcT++;
				}
			pbuSrc = rgbuSrc;
			}
		else
			pbuSrc = pbuBaseSrc;


		buSrc = *pbuSrc;

		//  unaligned bits on left side
		if (cPelsLeft != 0)
			{
			xDestCur += cPelsLeft;

			// do we need another Bltunit?
			if (cPelsNextSrc > 0 ||
				(cPelsNextSrc == 0 && xDestCur < xDestRight))
				{
				buSrcNext = *(++pbuSrc);
				buSrcData = buSrc << shSrc;
				buSrcData |= shSrc ? (buSrcNext >> (cbpbu - shSrc)) : 0;
				buSrc = buSrcNext;
				}
			else if((((xDest * cbppGrey) % cbpbu) + shSrc) < cbpbu)
				buSrcData = buSrc << shSrc;
			else
				buSrcData = buSrc >> (cbpbu - shSrc);
			WriteMask(*pbuDest, buSrcData, buMaskLeft);
			pbuDest++;
			}

		// src aligned
		if(shSrc == 0)
			{
			if(xDestCur + cppbuGrey <= xDestRight)
				{
				do
					{
					xDestCur = xDestCur + cppbuGrey;

					*pbuDest++ = *pbuSrc++;
					} while(xDestCur + cppbuGrey <= xDestRight);
				if(xDestCur < xDestRight)
					buSrc = *pbuSrc;
				}
			}
		else				// src not aligned
			while(xDestCur + cppbuGrey <= xDestRight)
				{
				xDestCur = xDestCur + cppbuGrey;

				buSrcNext = *(++pbuSrc);
				buSrcData = (buSrc << shSrc) | (buSrcNext >> (cbpbu - shSrc));
				*pbuDest = buSrcData;
				pbuDest++;
				buSrc = buSrcNext;
				}
		//  unaligned bits on right side
		if(xDestCur < xDestRight)
			{
			if(cPelsRight * cbppGrey > cbpbu - shSrc)
				{
				buSrcNext = *(++pbuSrc);
				buSrcData = (buSrc << shSrc) | (buSrcNext >> (cbpbu - shSrc));
				}
			else
				buSrcData = buSrc << shSrc;

			WriteMask(*pbuDest, buSrcData, buMaskRight);
			}

		pbuBaseDest += (dxScreen/cppbuGrey);
		pbuBaseSrc += rowBytes/(cbpbu/8);
		}
	_Win2ScreenAccess(0);
	return 1;
	}
#endif // WIN2DRAWBITMAPEX

#if defined(WIN2FILLRECT)
// UNDONE:  patterns...
void Win2FillHorizLine(short xDest, short dx, short y, Win2Color clr)
	{
	BltUnit buSrc;
	BltUnit buMskData;
	int xDestCur;
	int xDestRight;
	BltUnit *pbuDest;

	_Win2ScreenAccess(1);
	WinWindowToDisplayPt(&xDest, &y);
	xDestRight = xDest+dx;
	buSrc	= mpclrbuRep[clr & 0x3];
	pbuDest = ((BltUnit *)vwin2.pbGreyScreenBase)+(dxScreen/cppbuGrey)*y + xDest/(cppbuGrey);
	xDestCur = xDest;
	if (xDest % cppbuGrey)
		{
		int cPels;
		int cPelsRightShift;
		BltUnit buMask;

		cPels = cppbuGrey-(xDest%cppbuGrey);
		cPelsRightShift = (xDest*cbppGrey)%cbpbu;
		buMask = buAllOnes >> cPelsRightShift;
		if(xDest + cPels > xDestRight)
			{
			BltUnit cPelsRightDontTouch;

			cPelsRightDontTouch = xDest + cPels - xDestRight;
			buMask &= buAllOnes << (cPelsRightDontTouch * cbppGrey);
			cPels -= cPelsRightDontTouch;
			}
		xDestCur += cPels;
		WriteMask(*pbuDest,buSrc,buMask);
		pbuDest++;
		}
	if(xDestCur + cppbuGrey <= xDestRight)
		{
		do
			{
			xDestCur = xDestCur + cppbuGrey;

			*pbuDest++ = buSrc;
			} while(xDestCur + cppbuGrey <= xDestRight);
		}
	// now do right partial bltunit if necessary
	if(xDestCur < xDestRight)
		{
		int cPels;

		cPels = xDestRight - xDestCur;
		buMskData = buAllOnes << (cppbuGrey - cPels) * cbppGrey;
		WriteMask(*pbuDest,buSrc,buMskData);
		}
	_Win2ScreenAccess(0);
	}

int Win2FillRect(RectangleType *prc, int dxyCornder, int clr)
	{
	RectangleType rc;
	int y;

	rc = *prc;
	if (_Win2FClipRect(&rc))
		{
		for (y = 0; y < rc.extent.y; y++)
			Win2FillHorizLine(rc.topLeft.x, rc.extent.x, rc.topLeft.y+y, (Win2Color)clr);
		}
	return 1;
	}

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
void Win2EraseRectangle (RectanglePtr r, Word cornerDiam)
	{
	Win2FillRect(r, cornerDiam, 0);
	}

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
void Win2FillRectangle (RectanglePtr r, Word cornerDiam)
	{
	Win2FillRect(r, cornerDiam, vwin2.clr);
	}
#endif	// defined(WIN2FILLRECT)


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
Win2Color Win2SetColor(Win2Color clr)
	{
	Win2Color clrOld;

	clrOld = vwin2.clr;
	vwin2.clr = clr;
	return clrOld;
	}

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
Win2Color Win2SetBackgroundColor(Win2Color clr)
	{
	Win2Color clrOld;

	clrOld = vwin2.clrBackground;
	vwin2.clrBackground = clr;
	return clrOld;
	}

#ifdef OLD
WinHandle Win2SetDrawWindow(WinHandle winHandle)
	{
	vwin2.winHandle = winHandle;
	winHandle = WinSetDrawWindow(winHandle);
	return winHandle;
	}
#endif


#if defined(WIN2DRAWCHARS)
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
void Win2DrawChars(CharPtr chars, Word len, SWord x, SWord y)
	{
	char *pch;
	char *pchMac;
	FontPtr pfnt;
	Word *plocTbl;
	Word *ploc;
	FontCharInfoType *pfciTbl;
	FontCharInfoType *pfci;
	RectangleType rc;
	int xRight;

	rc.topLeft.x = x;
	rc.topLeft.y = y;
	rc.extent.x = dxScreen-x;
	rc.extent.y = FntCharHeight();

	if (_Win2FClipRect(&rc))
		{
		_Win2ScreenAccess(1);
		pfnt = FntGetFontPtr();

		xRight = rc.topLeft.x+rc.extent.x;
		pchMac = chars+len;
      plocTbl = ((Word *)(pfnt+1))+pfnt->rowWords*pfnt->fRectHeight;
		pfciTbl = (FontCharInfoType *) (((Word *)&pfnt->owTLoc)+pfnt->owTLoc);
		pchMac = chars+len-(*(chars+len-1)=='\n'?1:0);
		for (pch = chars; pch < pchMac && x < xRight; pch++)
			{
			int ch;
			int dch;
			int dxCharImage;
			int xCharImage;

			ch = *pch;
			if (ch < pfnt->firstChar || ch > pfnt->lastChar)
				{
	Missing:
				ch = pfnt->lastChar+1;
				}
			dch = ch-pfnt->firstChar;
			pfci = pfciTbl+dch;
			if (*(Word *)pfci == 0xffff)
				goto Missing;
			ploc = plocTbl+dch;
			xCharImage = *ploc;
			dxCharImage = *(ploc+1)-xCharImage;
			if (dxCharImage > 0)
				{
				if (x < rc.topLeft.x)
					{
					// check for partial on left
					if (x + dxCharImage >= rc.topLeft.x)
						xCharImage+=rc.topLeft.x-x;
					else
						goto SkipChar;
					}
				if (x + dxCharImage >= xRight)
					{
					// partial on right
					dxCharImage = xRight-x;
					}
				Win2DrawBitmapEx((Byte *)(pfnt+1), xCharImage, 0,  x, y, dxCharImage, pfnt->fRectHeight, pfnt->rowWords*2, 1);
				}
			// their WinDrawLine appears to be inclusive.  duh
	//		WinDrawGrayLine(x, yOut+pfnt->fRectHeight-1, x+pfci->width-1, yOut+pfnt->fRectHeight-1);
			// BUG!
SkipChar:
			x += pfci->width;
			}
		_Win2ScreenAccess(0);
		}
	}
#endif // WIN2DRAWCHARS

#if defined(WIN2DRAWLINE)
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
void Win2DrawLineEx(SWord x1, SWord y1, SWord x2, SWord y2, Win2Color clr)
	{
	int dxAbs;
	int dyAbs;
	int dxInc;
	int dyInc;
	int i;
	int dxStep;
	int dyStep;
	int x;
	int y;
	int wErr;
	int wErrIncNormal;
	int wErrIncSkip;
	int dxyLine;
	BltUnit buSrc;
	BltUnit *pbuDest;
	BltUnit mskDest;
	RectangleType rcClip;
	int xClipRight;
	int yClipBot;

	// ye olde bresenham
	dxAbs = x2-x1;
	dyAbs = y2-y1;

	if (dxAbs >= 0)
		dxInc = 1;
	else
		{
		dxInc = -1;
		dxAbs = -dxAbs;
		}
	if (dyAbs >= 0)
		dyInc = 1;
	else
		{
		dyInc = -1;
		dyAbs = -dyAbs;
		}
#ifdef WIN2FILLRECT
	// horiz line
	if (dyAbs == 0)
		{
		RectangleType rc;
		// do this before mapping to display point cause Win2FillHorizontalLine does that
		rc.topLeft.x = x1 < x2 ? x1 : x2+1;
		rc.topLeft.y = y1;
		rc.extent.x = dxAbs;
		rc.extent.y = 1;
		Win2FillRectangle(&rc, 0);
		return;
		}
#endif
	WinGetClip(&rcClip);
	buSrc = mpclrbuRep[clr];
	WinWindowToDisplayPt(&rcClip.topLeft.x, &rcClip.topLeft.y);
	xClipRight = rcClip.topLeft.x + rcClip.extent.x;
	yClipBot = rcClip.topLeft.y + rcClip.extent.y;

	WinWindowToDisplayPt(&x1, &y1);
	WinWindowToDisplayPt(&x2, &y2);
	_Win2ScreenAccess(1);
	x = x1;
	y = y1;

	// vert line
	if (dxAbs == 0)
		{
		int cbuInc;

		pbuDest = ((BltUnit *)vwin2.pbGreyScreenBase)+(dxScreen/cppbuGrey)*y + x1/(cppbuGrey);
		mskDest = 0x3<<((cppbuGrey-1-x1%cppbuGrey)*cbppGrey);
		cbuInc = (dxScreen/cppbuGrey)*dyInc;
		while (y != y2)
			{
			WriteMask(*pbuDest, buSrc, mskDest);
			pbuDest += cbuInc;
			y += dyInc;
			}
		goto Return;
		}

	if (dxAbs > dyAbs)
		{
		// x incs more
		dxStep = dxInc;
		dyStep = 0;
		wErr = dyInc*2-dxAbs;
		wErrIncNormal = dyAbs*2;
		wErrIncSkip = wErrIncNormal-2*dxAbs;
		dxyLine = dxAbs;
		}
	else
		{
		dxStep = 0;
		dyStep = dyInc;
		wErr = 2*dxInc-dyAbs;
		wErrIncNormal = dxAbs*2;
		wErrIncSkip = wErrIncNormal-2*dyAbs;
		dxyLine = dyAbs;
		}
	for (i = 0; i < dxyLine; i++)
		{
		// this could be optimized
		if (x >= rcClip.topLeft.x && x < xClipRight &&
			y >= rcClip.topLeft.y && y < yClipBot)
			{
			pbuDest = ((BltUnit *)vwin2.pbGreyScreenBase)+(dxScreen/cppbuGrey)*y + x/(cppbuGrey);
			mskDest = 0x3<<((cppbuGrey-1-x%cppbuGrey)*cbppGrey);
			WriteMask(*pbuDest, buSrc, mskDest);
			}
		if (wErr < 0)
			{
			x += dxStep;
			y += dyStep;
			wErr += wErrIncNormal;
			}
		else
			{
			x += dxInc;
			y += dyInc;
			wErr += wErrIncSkip;
			}
		}
Return:
	_Win2ScreenAccess(0);
	}

void Win2DrawLine(SWord x1, SWord y1, SWord x2, SWord y2)
	{
	Win2DrawLineEx(x1, y1, x2, y2, vwin2.clr);
	}
#endif // defined(WIN2DRAWLINE)


#ifdef OLD
int Win2Init()
	{
	return 1;
	}
#endif

void Win2BlatBufferToScreen()
{
  if (!vwin2.buffered)
    return;
  _Win2ScreenAccess(1);
  MemMove(vwin2.pbGreyRealScreenBase,vwin2.pbGreyScreenBase,cbGreyScreen);
  _Win2ScreenAccess(0);
}

void Win2DrawToBuffer(int drawToBuffer)
{
  if (!vwin2.buffered) return;
  if (vwin2.drawingToBuffer!=drawToBuffer) {
	  Byte* temp=vwin2.pbGreyRealScreenBase;
		vwin2.pbGreyRealScreenBase=vwin2.pbGreyScreenBase;
		vwin2.pbGreyScreenBase=temp;
		vwin2.drawingToBuffer=drawToBuffer;
	}
}

void Win2BlatAreaToScreen(int x,int y,int width,int height)
{
  if (vwin2.buffered) {
		int i,j;
		unsigned long *realScreen=(unsigned long *)vwin2.pbGreyRealScreenBase;
		unsigned long *screen=(unsigned long *)vwin2.pbGreyScreenBase;
		int xs=x/16-(x<0?1:0);
		int xe=(x+width)/16-(x+width<0?1:0);
		int ye=y+height;
		if (xe<0||xs>9||y>=160||ye<=0) return;
		if (xs<0) xs=0;
		if (xe>9) xe=9;
		if (ye>160) ye=160;
		if (y<0)
			y=0;
		else {
			screen+=y*10;
			realScreen+=y*10;
		}
		_Win2ScreenAccess(1);
		for(i=y;i<ye;i++) {
			for(j=xs;j<=xe;j++)
				*(realScreen+j)=*(screen+j);
			realScreen+=10;
			screen+=10;
		}
		_Win2ScreenAccess(0);
	}
}

void Win2ClearScreen()
{
  _Win2ScreenAccess(1);
	MemSet(vwin2.pbGreyScreenBase, cbGreyScreen, 0);
	_Win2ScreenAccess(0);
}

void Win2DrawImage(unsigned long *pics,int x,int y,int width,int height)
{
  int i,j,endy;

  unsigned long *screen=(unsigned long *)vwin2.pbGreyScreenBase;
  int xp=x/16-(x<0?1:0);
  int n=2*((x-xp*16)&0x0f);
  int m=32-n;
	int blocksPerLine=(width+15)/16;
  int endxp=xp+blocksPerLine;
	int endbits=(width%16)*2;
	unsigned long endmask=((1<<(endbits+1))-1)<<(32-endbits);

	if ((endy=y+height)>160) endy=160;

	if (endy<=0||y>=160||endxp<0||xp>=10)
		return;

	if (y<0) {
		pics+=blocksPerLine*(-y);
		y=0;
	}
	else
		screen+=y*10 ;

  _Win2ScreenAccess(1);
  for(i=y;i<endy;i++) {
    for(j=xp;j<endxp;j++) {
      unsigned long mask=(j==endxp-1?endmask:0xFFFFFFFF);
      if (j>=0&&j<10)
        *(screen+j)=(*(screen+j)&~(mask>>n))|(*pics>>n);
      if (j>=-1&&j<9)
				*(screen+j+1)=(*(screen+j+1)&~(mask<<m))|(*pics<<m);
      pics++;
    }
    screen+=10;
  }
  _Win2ScreenAccess(0);
}

void Win2DrawMaskedImage(unsigned long *pics,unsigned long *masks,int x,int y,int width,int height)
{
  int i,j,endy;

  unsigned long *screen=(unsigned long *)vwin2.pbGreyScreenBase;
  int xp=x/16-(x<0?1:0);
  int n=2*((x-xp*16)&0x0f);
  int m=32-n;
	int blocksPerLine=(width+15)/16;
  int endxp=xp+blocksPerLine;

  if ((endy=y+height)>160) endy=160;

	if (endy<=0||y>=160||endxp<0||xp>=10)
	  return;

	if (y<0) {
		pics+=blocksPerLine*(-y);
		y=0;
	}
	else
		screen+=y*10 ;

  _Win2ScreenAccess(1);
  for(i=y;i<endy;i++) {
    for(j=xp;j<endxp;j++) {
      if (j>=0&&j<10)
        *(screen+j)=(*(screen+j)&(~(*masks>>n)))|(*pics>>n);
			if (j>=-1&&j<9)
				*(screen+j+1)=(*(screen+j+1)&~(*masks<<m))|(*pics<<m);
      pics++;
      masks++;
    }
    screen+=10;
  }
  _Win2ScreenAccess(0);
}

int Win2IsGreyscale(void) {
  return(vwin2.pbGreyScreenBase!=0);
}
