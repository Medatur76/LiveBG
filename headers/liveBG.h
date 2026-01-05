#include <wayland-client.h>
#include "layer-shell-unstable-v1-client-protocol.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "font_8x16.h"

struct Color {
    int A;
    int R;
    int G;
    int B;
};
/*A type that holds the color values for a pixel.

The values inorder are:
A: Alpha
R: Red
G: Green
B: Blue

Each one can hold a value of 0 -> 255 or 0x00 -> 0xFF*/
typedef struct Color Pixel;

extern Pixel background;

struct Size {
    int w;
    int h;
};

extern uint8_t* pixel;

typedef void (*RenderEventListener)(struct Size);
typedef void (*TickEventListener)(void *input);

extern void appendRenderListener(RenderEventListener);
extern void appendTickListener(TickEventListener);

extern char* format(size_t, const char *__restrict, ...);

extern struct Size getCurrent();

extern void drawChar(int, int, char, struct Color, struct Size);
extern void drawText(int, int, const unsigned char*, struct Color, struct Size);
extern void drawRect(int, int, int, int, struct Color, struct Size);
extern void drawCircle(int, int, double, struct Color, struct Size);

extern int runWindow();