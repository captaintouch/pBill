#ifndef DRAWHELPER_H_
#define ALLOW_ACCESS_TO_INTERNALS_OF_BITMAPS
#define DRAWHELPER_H_
#include <PalmOS.h>
#include "colors.h"
#include "Picture.h"

typedef struct Coordinate {
    int x;
    int y;
} Coordinate;

Coordinate coordinate(int x, int y);

ImageData drawhelper_loadImage(UInt16 bitmapId);
void drawhelper_releaseImageData(ImageData imageData);
void drawhelper_drawPicture(Picture picture, Coordinate coordinate);
void drawhelper_applyForeColor(AppColor color);
void drawhelper_applyTextColor(AppColor color);
void drawhelper_applyBackgroundColor(AppColor color);
void drawhelper_drawText(char *text, Coordinate position);
void drawhelper_drawTextWithLength(char *text, int length, Coordinate position);

#endif