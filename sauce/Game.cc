#include "objects.h"
#include "resources.h"
#include <Font.h>
#include <StringMgr.h>
#include <SysUtils.h>
#include "drawhelper.h"

#define endgamestr "Module pBill has caused a segmentation fault at memory address 097E:F1A0.\nCore dumped.\n\n We apologize for the inconvenience."

Picture pause;
Horde bill;
Network net;
Library OS;
Bucket bucket;
Spark spark;
Game game;
UI ui;
SaveGameType savegame;

int Game::RAND(int lb, int ub) {
    return (SysRandom(0) % (ub - lb + 1) + lb);
}

int Game::MAX(int x, int y) {
    return (x > y ? x : y);
}

int Game::MIN(int x, int y) {
    return (x < y ? x : y);
}

int Game::ABS(int x) {
    return (x < 0 ? -x : x);
}

int Game::INTERSECT(int x1, int y1, int w1, int h1, int x2, int y2, int w2,
                    int h2) {
    return (((x2 - x1 <= w1 && x2 - x1 >= 0) || (x1 - x2 <= w2 && x1 - x2 >= 0)) && ((y2 - y1 <= h1 && y2 - y1 >= 0) || (y1 - y2 <= h2 && y1 - y2 >= 0)));
}

void Game::setup_level(unsigned int lev) {
    ui.reset();
    level = lev;
    bill.setup();
    grabbed = EMPTY;
    ui.set_cursor(DEFAULTC);
    net.setup();
    iteration = efficiency = 0;
}

void Game::start(unsigned int lev) {
    setup_level(lev);
    if (savegame.playing) {
        score = savegame.score;
        state = PAUSE;
    } else {
        state = PLAYING;
        score = 0;
        savegame.score = score;
        savegame.playing = 1;
        savegame.level = lev;
    }
    refresh();
}

void Game::draw_info() {
    static char str[80];
    StrPrintF(str, "Bill:%d/%d  Sys:%d/%d/%d  Lev:%d  Sc:%lu",
              bill.on_screen, bill.off_screen, net.base, net.off,
              net.win, level, score);
    drawhelper_applyBackgroundColor(CLOUDS);
    drawhelper_applyTextColor(ASBESTOS);
    ui.draw_str(str, 5, 149);
}

void Game::update_score(int action) {
    switch (action) {
    case ENDLEVEL:
        score += (level * efficiency / iteration);
        break;
    default:
        score += (action * action * BILLPOINTS);
    }
}

void Game::warp_to_level(unsigned int lev) {
    if (state == PLAYING) {
        if (lev <= level)
            return;
        setup_level(lev);
    } else {
        if (lev <= 0)
            return;
        start(lev);
    }
}

void Game::button_press(int x, int y) {
    int i, counter = 0, flag = 0;
    if (state != PLAYING)
        return;
    ui.set_cursor(DOWNC);
    if (x >= 150 && y <= 10) {
        state = PAUSE;
        refresh();
        return;
    }
    if (bucket.clicked(x, y)) {
        ui.set_cursor(BUCKETC);
        grabbed = BUCKET;
    }
    for (i = 0; i < bill.MAX_BILLS && !flag; i++) {
        if (bill.list[i].state == bill.list[i].OFF || bill.list[i].state == bill.list[i].DYING)
            continue;
        if (bill.list[i].state == bill.list[i].STRAY &&
            bill.list[i].clickedstray(x, y)) {
            ui.set_cursor(bill.list[i].cargo);
            grabbed = i;
            flag = 1;
        } else if (bill.list[i].state != bill.list[i].STRAY &&
                   bill.list[i].clicked(x, y)) {
            if (bill.list[i].state == bill.list[i].AT)
                net.computers[bill.list[i].target_c].busy = 0;
            bill.list[i].index = -1;
            bill.list[i].cels = bill.dcels;
            bill.list[i].x_offset = -1;
            bill.list[i].y_offset = -7;
            bill.list[i].state = bill.list[i].DYING;
            counter++;
        }
    }
    if (counter)
        update_score(counter);
}

void Game::button_release(int x, int y) {
    int i;
    ui.set_cursor(DEFAULTC);
    if (state != PLAYING || grabbed == EMPTY)
        return;
    if (grabbed == BUCKET) {
        grabbed = EMPTY;
        for (i = 0; i < net.ncables; i++)
            if (net.cables[i].onspark(x, y)) {
                net.cables[i].active = 0;
                net.cables[i].delay = spark.delay(level);
            }
        return;
    }
    for (i = 0; i < net.units; i++)
        if (net.computers[i].oncomputer(x, y) &&
            net.computers[i].compatible(bill.list[grabbed].cargo) &&
            (net.computers[i].os == OS.WINGDOWS ||
             net.computers[i].os == OS.OFF)) {
            net.base++;
            if (net.computers[i].os == OS.WINGDOWS)
                net.win--;
            else
                net.off--;
            net.computers[i].os = bill.list[grabbed].cargo;
            bill.list[grabbed].state = bill.list[grabbed].OFF;
            grabbed = EMPTY;
            return;
        }
    grabbed = EMPTY;
}

void Game::refresh() {
    if (state == SETUP) {
        return;
    } else if (state == END) {
        net.toasters();
        net.drawStatic();
        net.drawDynamic();
        ui.show_msg(endgamestr);
    } else {
       if (ui.drawInBackground()) {
            net.drawStatic();
            ui.draw(pause, 150, 0);
            ui.endDrawInBackground();
        }
        net.drawDynamic();
        bucket.draw();
        bill.draw();
        draw_info();
        if (state == BETWEEN) {
            ui.show_scorebox(level, score);
        } else if (state == PAUSE) {
            ui.show_msg("Paused");
        }
    }
}

void Game::update() {
    if (state == PLAYING) {
        net.update();
        efficiency += ((100 * net.base - 10 * net.win) / net.units);
        bill.update();
        if (!(bill.on_screen + bill.off_screen)) { // LEVEL COMPLETE
            ui.set_cursor(NONE);
            update_score(ENDLEVEL);
            state = BETWEEN;
            savegame.score = score;
            savegame.level = level;
        } else if ((net.base + net.off) <= 1) { // GAME OVER
            ui.set_cursor(NONE);
            state = END;
            savegame.playing = 0;
            ui.clear();
            net.toasters();
            net.drawStatic();
            net.drawDynamic();
            ui.refresh();
            ui.pause(1500);
            ui.show_msg(endgamestr);
            ui.refresh();
            return;
        }
        iteration++;
    }
}

void Game::init() {
    bill.load_pix();
    OS.load_pix();
    net.load_pix();
    bucket.load_pix();
    spark.load_pix();
    pause.load(GFX_RES_EXTRAS + 3, GFX_RES_EXTRAS_WIDTH, GFX_RES_EXTRAS_HEIGHT);
}

void Game::releaseImages() {
    bill.release_pix();
    OS.release_pix();
    net.release_pix();
    bucket.release_pix();
    spark.release_pix();
    pause.release();
}
