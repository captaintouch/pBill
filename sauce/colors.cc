#include "colors.h"
#include <PalmOS.h>

IndexedColorType colors_reference[3];

static RGBColorType colors_rgb(Int16 red, Int16 green, Int16 blue) {
    RGBColorType color;
    color.r = red;
    color.g = green;
    color.b = blue;
    return color;
}

static RGBColorType colors_bsodBlue() {
    return colors_rgb(0, 0, 255);
}

static RGBColorType colors_clouds() {
    return colors_rgb(255, 255, 255);
}

static RGBColorType colors_asbestos() {
    return colors_rgb(127, 140, 141);
}

void colors_setupReferenceColors(Boolean colorSupport, UInt32 depth) {
    RGBColorType color;
    if (colorSupport) {
        color = colors_bsodBlue();
        colors_reference[BSODBLUE] = WinRGBToIndex(&color);
        color = colors_clouds();
        colors_reference[CLOUDS] = WinRGBToIndex(&color);
        color = colors_asbestos();
        colors_reference[ASBESTOS] = WinRGBToIndex(&color);
    } else {
        int i;
        colors_reference[BSODBLUE] = 6;
        colors_reference[CLOUDS] = 0;
        colors_reference[ASBESTOS] = 13;

        if (depth == 8) {
            for (i = 0; i < 7; i++) {
                colors_reference[i] *= 2;
            }
        }
    }
}
