#include "objects.h"
#include "resources.h"

/*sets up network for each level*/
void Network::setup() {
    int i;
    units = on(game.level);
    for (i = 0; i < units; i++)
        if (!net.computers[i].setup(i)) {
            units = i - 1;
            break;
        }
    base = units;
    off = win = 0;
    ncables = game.MIN(game.level, units / 2);
    for (i = 0; i < ncables; i++)
        cables[i].setup();
}

/*redraws the computers at their location with the proper image*/
void Network::drawStatic() {
    int i;
    for (i = 0; i < ncables; i++)
        cables[i].draw();
    for (i = 0; i < units; i++)
        computers[i].draw(true);
}

void Network::drawDynamic() {
    int i;
    for (i = 0; i < units; i++)
        computers[i].draw(false);
}


void Network::update() {
    for (int i = 0; i < ncables; i++)
        cables[i].update();
}

void Network::toasters() {
    for (int i = 0; i < units; i++) {
        computers[i].type = computers[i].TOASTER;
        computers[i].os = OS.OFF;
    }
    ncables = 0;
}

int Network::on(int lev) {
    return game.MIN(8 + lev, MAX_COMPUTERS);
}

void Network::load_pix() {
    int i;
    for (i = 0; i <= NUM_SYS; i++)
        pictures[i].load(GFX_RES_SYSTEMS + i, GFX_RES_SYSTEMS_WIDTH, GFX_RES_SYSTEMS_HEIGHT);
    width = pictures[0].width;
    height = pictures[0].height;
}

void Network::release_pix() {
    int i;
    for (i = 0; i <= NUM_SYS; i++)
        pictures[i].release();
}
