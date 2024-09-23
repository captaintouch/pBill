#include <StringMgr.h>
#include <Common.h>
#include <System/SysAll.h>
#include <UI/UIAll.h>

#include "objects.h"
#define TYPE 'HISC'

Scorelist hscores;

void Scorelist::reset() {
	int i;
	for(i=0;i<NUM_SCORES;i++) {
		scores[i]=(NUM_SCORES-i)*50;
		levels[i]=1;
		DateSecondsToDate(TimGetSeconds(),&dates[i]);
	}
	changed=1;
}

void Scorelist::load() {
  UInt          index = 0;
  VoidHand      RecHandle;
  VoidPtr       RecPointer;
  Err           err;
  DmOpenRef     pmDB;

  // Create database, if it doesn't exist, and save default game status.
  if (!(pmDB = DmOpenDatabaseByTypeCreator(TYPE,CREATOR, dmModeReadOnly))) {
    if ((err = DmCreateDatabase(0, "pBillDB", CREATOR, TYPE, false)))
      return;
    pmDB = DmOpenDatabaseByTypeCreator(TYPE,CREATOR, dmModeWrite);

    reset();

    RecHandle = DmNewRecord(pmDB, &index, sizeof(hscores));
    DmWrite(MemHandleLock(RecHandle), 0, &hscores, sizeof(hscores));
    MemHandleUnlock(RecHandle);
    DmReleaseRecord(pmDB, index, true);
  } else {
		// Load a saved game status.
		RecHandle = DmGetRecord(pmDB, 0);
		RecPointer = MemHandleLock(RecHandle);
		MemMove(&hscores, RecPointer, sizeof(hscores));
		MemHandleUnlock(RecHandle);
		DmReleaseRecord(pmDB, 0, true);
  }
  DmCloseDatabase(pmDB);
  changed=0;
}

void Scorelist::save () {
	if (changed) {
	  DmOpenRef     pmDB;
		// Create database, if it doesn't exist, and save default game status.
		if (pmDB = DmOpenDatabaseByTypeCreator(TYPE,CREATOR, dmModeWrite)) {
			VoidPtr p = MemHandleLock(DmGetRecord(pmDB, 0));
			DmWrite(p, 0, &hscores, sizeof(hscores));
			MemPtrUnlock(p);
			DmReleaseRecord(pmDB, 0, true);
			DmCloseDatabase(pmDB);
		}
  }
}

/*  Add new high score to list   */
int Scorelist::add () {
	unsigned long score=game.score;
	unsigned level=game.level;
	int i,j;
	if (score<=scores[NUM_SCORES-1])
	  return -1;
	for(i=0;i<NUM_SCORES&&scores[i]>=score;i++);
	for(j=NUM_SCORES-2;j>=i;j--) {
		scores[j+1]=scores[j];
		levels[j+1]=levels[j];
		dates[j+1]=dates[j];
  }
  changed=1;
  scores[i]=score;
  levels[i]=level;
  DateSecondsToDate(TimGetSeconds(),&dates[i]);
  return i;
}

void Scorelist::draw(int bold) {
	DateFormatType dateFormat=(DateFormatType)PrefGetPreference(prefDateFormat);
	FontID lastFont;
	int i;
	int y=90;
	char s[20];
	RectangleType rect;
	RctSetRectangle(&rect,5,y,150,FntLineHeight()*NUM_SCORES);
	WinEraseRectangle(&rect,0);
	for(i=0;i<NUM_SCORES;i++) {
		if (i==bold)
		  lastFont=FntSetFont(boldFont);
		StrPrintF(s,"%u",levels[i]);
		WinDrawChars(s,StrLen(s),35-FntLineWidth(s,StrLen(s)),y);
		StrPrintF(s,"%lu",scores[i]);
		WinDrawChars(s,StrLen(s),75-FntLineWidth(s,StrLen(s)),y);
		DateToAscii(dates[i].month,dates[i].day,dates[i].year+1904,dateFormat,s);
		WinDrawChars(s,StrLen(s),85,y);
		if (i==bold)
		  FntSetFont(lastFont);
		y+=FntLineHeight();
  }
}

