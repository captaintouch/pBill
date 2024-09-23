#include <PalmOS.h>

#include "colors.h"
#include "deviceinfo.h"
#include "objects.h"
#include "oldTypes.h"
#include "pBill.h"

#include <StringMgr.h>

int warp_level = 1;

static void startGame(int level) {
    FrmCloseAllForms();
    RectangleType rect;
    RctSetRectangle(&rect, 0, 0, 160, 160);
    WinEraseRectangle(&rect, 0);
    game.start(level);
}

static Boolean WebFormHandleEvent(EventPtr event) {
    Boolean handled = false;

    switch (event->eType) {
    case ctlSelectEvent:
        if (event->data.ctlEnter.controlID == webCloseBtn) {
            FrmReturnToForm(0);
            handled = true;
        }
        break;
    }
    return handled;
}

static Boolean WarpFormHandleEvent(EventPtr event) {
    Boolean handled = false;
    char lev[10];

    switch (event->eType) {
    case ctlRepeatEvent:
        switch (event->data.ctlEnter.controlID) {
        case levelUp:
            warp_level++;
            break;
        case levelDown:
            warp_level--;
            if (warp_level < 1)
                warp_level = 1;
            break;
        }
        StrPrintF(lev, "%2d", warp_level);
        WinDrawChars(lev, StrLen(lev), 125 - FntCharsWidth(lev, StrLen(lev)), 20);
        break;
    case ctlSelectEvent:
        if (event->data.ctlEnter.controlID == warpOK) {
            startGame(warp_level);
            handled = true;
        } else if (event->data.ctlEnter.controlID == warpCancel) {
            FrmReturnToForm(0);
            handled = true;
        }
        break;
    }
    return handled;
}

static Boolean MainFormHandleEvent(EventPtr event) {
    Boolean handled = false;

    switch (event->eType) {
    case ctlSelectEvent:
        if (event->data.ctlEnter.controlID == newGameBtn) {
            startGame(1);
            handled = true;
        } else if (event->data.ctlEnter.controlID == warpBtn) {
            FrmPopupForm(warpForm);
        } else if (event->data.ctlEnter.controlID == webBtn) {
            FrmPopupForm(webForm);
        }
        break;
    case menuEvent:
        switch (event->data.menu.itemID) {
        case newGameMenu:
            startGame(1);
            handled = true;
            break;
        case warpMenu:
            FrmPopupForm(warpForm);
            break;
        case hiScoreMenu:
            hscores.reset();
            FrmUpdateForm(mainForm, frmRedrawUpdateCode);
            break;
        case storyMenu:
            FrmHelp(storyStr);
            break;
        case rulesMenu:
            FrmHelp(rulesStr);
            break;
        case aboutMenu:
            FrmAlert(aboutAlert);
            break;
        }
        break;
    }
    return handled;
}

static UInt32 ApplyScreenMode() {
    Int32 oldDepth = deviceinfo_currentDepth();
    UInt32 depth = deviceinfo_maxDepth();
    if (depth < 4 || WinScreenMode(winScreenModeSet, NULL, NULL, &depth, NULL) != errNone) {
        ErrFatalDisplay("Unsupported device");
    }

    colors_setupReferenceColors(deviceinfo_colorSupported(), depth);
    return oldDepth;
}

static void LoadApplicationState() {
    UInt16 prefsSize;
    Int16 prefsVersion = noPreferenceFound;
    prefsSize = sizeof(SaveGameType);
    prefsVersion = PrefGetAppPreferences(CREATOR, 0, &savegame, &prefsSize, true);

    if (prefsVersion > 1)
        prefsVersion = noPreferenceFound;
    if (prefsVersion == noPreferenceFound) {
        savegame.playing = 0;
        savegame.score = 0;
        savegame.level = 1;
    }
}

static void SaveApplicationState(void) {
    PrefSetAppPreferences(CREATOR, 0, 1, &savegame, sizeof(SaveGameType), true);
}

static Boolean Win2PreFilterEvent(const EventType *pevt) {
    if (pevt->eType == keyDownEvent && (pevt->data.keyDown.modifiers & commandKeyMask)) {
        switch (pevt->data.keyDown.chr) {
        default:
            break;
        case keyboardChr:
        case graffitiReferenceChr: // popup the Graffiti reference
        case keyboardAlphaChr:     // popup the keyboard in alpha mode
        case keyboardNumericChr:   // popup the keyboard in number mode
            return 1;              // eat, don't switch to mono
        case hardPowerChr:
        case backlightChr:
            // don't switch to mono, but don't eat either
            break;
        }
    }
    return 0;
}

UInt32 PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags) {
    unsigned short err;
    EventType e;
    FormType *pfrm;
    UInt16 formID;
    unsigned long next_draw_ticks = 0;
    unsigned long tick_rate = 350 * SysTicksPerSecond() / 1000;
    int hiScore = -1;
    UInt32 oldDepth;

    if (cmd == sysAppLaunchCmdNormalLaunch) {
        oldDepth = ApplyScreenMode();
        LoadApplicationState();

        game.init();
        hscores.load();

        if (savegame.playing)
            startGame(savegame.level);
        else {
            game.state = game.SETUP;
            FrmGotoForm(mainForm);
        }

        while (1) {
            if (game.state == game.PLAYING)
                EvtGetEvent(&e, 0);
            else
                EvtGetEvent(&e, evtWaitForever);

            if (e.eType == appStopEvent) {
                game.releaseImages();
                ui.release();
                SaveApplicationState();
                hscores.save();
                pfrm = FrmGetActiveForm();
                if (pfrm != NULL) {
                    FrmDeleteForm(pfrm);
                    pfrm = NULL;
                }
                FrmCloseAllForms();
                WinScreenMode(winScreenModeSet, NULL, NULL, &oldDepth, NULL);
                return 0;
            }

            if (game.state != game.SETUP) {
                if (Win2PreFilterEvent(&e))
                    continue;
                if (e.eType == keyDownEvent && e.data.keyDown.chr == menuChr) {
                    game.state = game.SETUP;
                    savegame.playing = 0;
                    hiScore = hscores.add();
                    FrmGotoForm(mainForm);
                    continue;
                }
            }

            if (SysHandleEvent(&e))
                continue;

            if (game.state == game.SETUP && MenuHandleEvent(0, &e, &err))
                continue;

            switch (e.eType) {

            case frmLoadEvent:
                formID = e.data.frmLoad.formID;
                pfrm = FrmInitForm(formID);
                FrmSetActiveForm(pfrm);
                if (formID == mainForm) {
                    FrmSetEventHandler(pfrm, MainFormHandleEvent);
                } else if (formID == warpForm) {
                    FrmSetEventHandler(pfrm, WarpFormHandleEvent);
                } else if (formID == webForm) {
                    FrmSetEventHandler(pfrm, WebFormHandleEvent);
                }
                break;

            case frmOpenEvent:
            case frmUpdateEvent:
                pfrm = FrmGetActiveForm();
                FrmDrawForm(pfrm);
                switch (FrmGetActiveFormID()) {
                case mainForm:
                    hscores.draw(hiScore);
                    if (hiScore != -1)
                        FrmAlert(hiScoreAlert);
                    hiScore = -1;
                    break;
                case warpForm:
                    char lev[10];
                    StrPrintF(lev, "%2d", warp_level);
                    WinDrawChars(lev, StrLen(lev), 125 - FntCharsWidth(lev, StrLen(lev)), 20);
                    break;
                default:
                    break;
                }
                break;

            case penDownEvent:
                switch (game.state) {
                case game.SETUP:
                    FrmDispatchEvent(&e);
                    break;
                case game.BETWEEN:
                    game.state = game.PLAYING;
                    ui.clear();
                    ui.refresh();
                    game.setup_level(++game.level);
                    savegame.score = game.score;
                    savegame.level = game.level;
                    game.refresh();
                    break;
                case game.PAUSE:
                    game.state = game.PLAYING;
                    game.refresh();
                    break;
                case game.END:
                    game.state = game.SETUP;
                    hiScore = hscores.add();
                    FrmGotoForm(mainForm);

                    break;
                default:
                    game.button_press(e.screenX, e.screenY);
                    break;
                }
                break;

            case penMoveEvent:
                if (game.state == game.PLAYING)
                    ui.update_cursor_position(e.screenX, e.screenY);
                break;
            case penUpEvent:
                if (game.state == game.PLAYING)
                    game.button_release(e.screenX, e.screenY);
                break;

            default:
                if (game.state == game.SETUP)
                    FrmDispatchEvent(&e);
            }

            /* do some kind of 50Hz timer event */
            if (game.state == game.PLAYING) {
                long ticksToGo = next_draw_ticks - TimGetTicks();
                Boolean didUpdate = false;
                if (ticksToGo < 0) {
                    game.update();
                    ui.clear();
                    game.refresh();
                    didUpdate = true;

                    if (ticksToGo < -tick_rate) {
                        next_draw_ticks = TimGetTicks();
                    }
                    next_draw_ticks += tick_rate;
                }
                if (ui.didMoveCursor() || didUpdate) {
                    ui.refresh();
                }
            }
        }
    }
    return 0;
}
