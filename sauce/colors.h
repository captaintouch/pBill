#ifndef COLORS_H_
#define COLORS_H_

#include <PalmOS.h>

typedef enum {
    BSODBLUE,
    CLOUDS,
    ASBESTOS
} AppColor;

extern IndexedColorType colors_reference[3];

void colors_setupReferenceColors(Boolean colorSupport, UInt32 depth);

#endif