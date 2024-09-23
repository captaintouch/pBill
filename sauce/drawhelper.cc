#include "drawhelper.h"
#include "Picture.h"
#include "colors.h"
#include <PalmOS.h>

Coordinate coordinate(int x, int y) {
    Coordinate coordinate;
    coordinate.x = x;
    coordinate.y = y;
    return coordinate;
}

ImageData drawhelper_loadImage(UInt16 bitmapId) {
    MemHandle bitmapH;
    ImageData imageDataContainer;

    bitmapH = DmGetResource(bitmapRsc, bitmapId);
    if (bitmapH) {
        BitmapPtr bitmap = (BitmapPtr)MemHandleLock(bitmapH);
        imageDataContainer.bitmapPtr = bitmap;
        imageDataContainer.resource = bitmapH;
    }
    return imageDataContainer;
}

void drawhelper_releaseImageData(ImageData imageData) {
    MemHandleUnlock(imageData.resource);
    DmReleaseResource(imageData.resource);
}

void drawhelper_drawPicture(Picture picture, Coordinate coordinate) {
    WinDrawBitmap(picture.imageData.bitmapPtr, coordinate.x, coordinate.y);
}

void drawhelper_applyForeColor(AppColor color) {
    WinSetForeColor(colors_reference[color]);
}

void drawhelper_applyTextColor(AppColor color) {
    WinSetTextColor(colors_reference[color]);
}

void drawhelper_applyBackgroundColor(AppColor color) {
    WinSetBackColor(colors_reference[color]);
}

void drawhelper_drawText(char *text, Coordinate position) {
    WinDrawChars(text, StrLen(text), position.x, position.y);
}

void drawhelper_drawTextWithLength(char *text, int length, Coordinate position) {
    WinDrawChars(text, length, position.x, position.y);
}
