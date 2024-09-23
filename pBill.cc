#include <Common.h>
#include <System/SysAll.h>
#include <UI/UIAll.h>
#include <System/SysEvtMgr.h>

#include "win2.h"
#include "pBill.h"
#include "objects.h"
#include "pics.h"

#include <StringMgr.h>

int warp_level=1;

static void startGame(int level) {
	FrmCloseAllForms();
	RectangleType rect;
	RctSetRectangle(&rect,0,0,160,160);
	WinEraseRectangle(&rect,0);
	if (Win2SetGreyscale(1) != 0) {
		FrmCustomAlert(alt_err, "Out of memory!",0,0);
		FrmGotoForm(mainForm);
	} else
		game.start(level);
}

static Boolean WarpFormHandleEvent(EventPtr event) {
	FormPtr frm;
	Boolean handled = false;
	char lev[10];

	switch (event->eType) {
	  case ctlRepeatEvent:
			switch(event->data.ctlEnter.controlID) {
				case levelUp:
					warp_level++;
					break;
				case levelDown:
					warp_level--;
					if(warp_level<1) warp_level=1;
					break;
		  }
		  StrPrintF(lev,"%2d",warp_level);
		  WinDrawChars(lev,StrLen(lev),125-FntCharsWidth(lev,StrLen(lev)),20);
		  break;
		case ctlSelectEvent:
		  if (event->data.ctlEnter.controlID == warpOK) {
				startGame(warp_level);
				handled=true;
		  } else
			if (event->data.ctlEnter.controlID == warpCancel) {
			  FrmReturnToForm(0);
			  handled=true;
			}
			break;
  }
  return handled;
}

static Boolean MainFormHandleEvent(EventPtr event)
{
  FormPtr frm;
  Boolean handled = false;
  int i;
  char str[4];

  switch (event->eType) {
/*

*/
  case ctlSelectEvent:
    if(event->data.ctlEnter.controlID == newGameBtn) {
			startGame(1);
			handled = true;
    }
    else
    if(event->data.ctlEnter.controlID == warpBtn) {
			FrmPopupForm(warpForm);
	  }
    break;
  case menuEvent:
    switch (event->data.menu.itemID)
    {
      case newGameMenu:
        startGame(1);
        handled=true;
        break;
      case warpMenu:
        FrmPopupForm(warpForm);
        break;
      case hiScoreMenu:
        hscores.reset();
        FrmUpdateForm(mainForm,frmRedrawUpdateCode);
        break;
      case storyMenu:
    	  FrmHelp (storyStr);
			  break;
      case rulesMenu:
    	  FrmHelp (rulesStr);
			  break;
      case aboutMenu:
			  FrmAlert (aboutAlert);
			  break;
    }
    break;
  }
  return handled;
}

static void LoadApplicationState()
{
  Word	prefsSize;
  SWord	prefsVersion = noPreferenceFound;
  int i;
	
  prefsSize = sizeof (SaveGameType);
  prefsVersion = PrefGetAppPreferences (CREATOR, 0, &savegame, &prefsSize, true);
	
  if (prefsVersion > 1)
    prefsVersion = noPreferenceFound;
  if (prefsVersion == noPreferenceFound)
  {
    savegame.playing=0;
		savegame.score=0;
		savegame.level=1;
  }
}

static void SaveApplicationState(void)
{
  PrefSetAppPreferences(CREATOR, 0, 1, &savegame, sizeof(SaveGameType), true);
}

DWord PilotMain(Word cmd, Ptr cmdPBP, Word launchFlags) {
  unsigned short err;
  EventType e;
  FormType *pfrm;
  FormPtr mainFrm,warpFrm;
  Boolean fMainWindowActive;
  Word formID;
  Word  x,y;
  unsigned long  next_draw_ticks=0;
  unsigned long tick_rate=350*SysTicksPerSecond()/1000;
  Word  size;
  SystemPreferencesType sysPrefs;
  int tx,ty,hiScore=-1,quit=0;

  if (!cmd) {

		LoadApplicationState();

    game.init();
    hscores.load();

    if (savegame.playing)
			startGame(savegame.level);
		else
		{
		  game.state=game.SETUP;
      FrmGotoForm(mainForm);		  
		}

    while(1) {
			if (game.state==game.PLAYING)
			  EvtGetEvent(&e, game.MAX(next_draw_ticks-TimGetTicks(),0));
			else
        EvtGetEvent(&e, evtWaitForever);

      if(game.state!=game.SETUP) {
      	if (Win2PreFilterEvent(&e))
       	  continue;

				if (e.eType==keyDownEvent&&e.data.keyDown.chr==menuChr) {
					game.state=game.SETUP;
					savegame.playing=0;
					Win2SetMono();
					hiScore=hscores.add();
					FrmGotoForm(mainForm);
					continue;
				}
			}

      if (SysHandleEvent(&e))
      	continue;

      if (game.state==game.SETUP && MenuHandleEvent((void *)0, &e, &err))
      	continue;

      if (e.eType==appStopEvent) {
				if(game.state != game.SETUP)
				  Win2SetMono();
				SaveApplicationState();
				hscores.save();
				return 0;
		  }
      /* formerly in winEnterEvent: */

      /* left greyscale mode? (may happen due to alarm etc) */
      if((game.state!=game.SETUP)&&(!Win2IsGreyscale())) {
      	if (Win2SetGreyscale(1) != 0) {
      	  FrmCustomAlert(alt_err, "Out of memory!",0,0);
       	  game.state=game.SETUP;
      	} else {
      	  /* remove alert or whatever (redraw everything) */
      	  game.refresh();
      	}
      }

      switch (e.eType) {

				case frmLoadEvent:
				  formID=e.data.frmLoad.formID;
					pfrm = FrmInitForm(formID);
					FrmSetActiveForm(pfrm);
					if (formID==mainForm) {
					  mainFrm=pfrm;
					  FrmSetEventHandler(pfrm, MainFormHandleEvent);
				  } else if (formID==warpForm) {
					  warpFrm=pfrm;
					  FrmSetEventHandler(pfrm, WarpFormHandleEvent);
					}
					break;

				case frmOpenEvent:
				case frmUpdateEvent:
					pfrm = FrmGetActiveForm();
					FrmDrawForm(pfrm);
					if (pfrm==mainFrm) {
						hscores.draw(hiScore);
						if (hiScore!=-1)
							FrmAlert(hiScoreAlert);
						hiScore=-1;
				  }
				  else if (pfrm==warpFrm) {
						char lev[10];
						StrPrintF(lev,"%2d",warp_level);
						WinDrawChars(lev,StrLen(lev),125-FntCharsWidth(lev,StrLen(lev)),20);
				  }
					break;

				case penDownEvent:
					switch(game.state) {
						case game.SETUP:
							FrmDispatchEvent(&e);
							break;
						case game.BETWEEN:
							game.state=game.PLAYING;
							ui.clear();
							ui.refresh();
							game.setup_level(++game.level);
							savegame.score=game.score;
							savegame.level=game.level;
							game.refresh();
							break;
						case game.PAUSE:
							game.state=game.PLAYING;
							game.refresh();
							break;
						case game.END:
							game.state=game.SETUP;
							Win2SetMono();
							hiScore=hscores.add();
							FrmGotoForm(mainForm);

							break;
						default:
							game.button_press(e.screenX,e.screenY);
					}
					break;

				case penMoveEvent:
				  if (game.state==game.PLAYING)
  					ui.draw_cursor(e.screenX,e.screenY);
					break;
				case penUpEvent:
				  if (game.state==game.PLAYING)
  					game.button_release(e.screenX,e.screenY);
					break;

		/* tap menu soft key to stop game */


					/* save name of current database
					MemMove(prefs.db_name, db_name[db_no], 32);
					prefs.version=PREFS_VERSION;
					prefs.sound=sound_on;
					prefs.tilt =tilt_on;
					PrefSetAppPreferences( CREATOR, 0, 1, &prefs, sizeof(prefs), true);
					settings(true, &max_enabled, &level_no);
					*/



				default:
				Dft:
					if(game.state==game.SETUP)
						FrmDispatchEvent(&e);
			}
			/* do some kind of 50Hz timer event */
			if(game.state==game.PLAYING) {
			  long ticksToGo=next_draw_ticks-TimGetTicks();
				if(ticksToGo<0) {
				//	char s[40];
				  game.update();
				//  StrPrintF(s,"%lu %ld %d",next_draw_ticks,ticksToGo,tick_rate);
				 // Win2DrawToBuffer(0);
				  //Win2DrawChars(s,StrLen(s),40,0);

					if (ticksToGo<-tick_rate) {
					  next_draw_ticks=TimGetTicks();
					  //Win2DrawChars("fuck",4,40,30);
				  }
				  		//		  Win2DrawToBuffer(1);
					next_draw_ticks+=tick_rate;
				}
			}
  	}
  }
  return 0;
}





