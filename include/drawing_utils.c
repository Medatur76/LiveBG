#ifndef LIVEBG
#define LIVEBG
#include "../headers/liveBG.h"
#endif

void drawChar(int x, int y, char c, struct Color color, struct Size current) {
    const int fw = 8;
    const int fh = 16;

    if (c >= 256)
        return;
    
    for (int row = 0; row < fh; row++) {
        uint8_t row_bits =
            fontdata_8x16[(unsigned char)c * fh + row];

        for (int col = 0; col < fw; col++) {
            if (row_bits & 0x80) {
                int cx = x + col;
                int cy = y + row;

                if ((unsigned)cx < (unsigned)current.w && (unsigned)cy < (unsigned)current.h) {

                    int i = (cy * current.w + cx) * 4;
                    pixel[i + 0] = color.B;  /* B */
                    pixel[i + 1] = color.G;  /* G */
                    pixel[i + 2] = color.R;  /* R */
                    pixel[i + 3] = color.A;  /* A */
                }
            }
            row_bits <<= 1;
        }
    }
}

void drawText(int x, int y, const unsigned char *s, struct Color color, struct Size current) {
    int cx = x, cy = y;
    for (size_t i = 0; s[i]; i++) {
        if (s[i] == 0x0A) {
            cy += FONT_H;
            cx = x - FONT_W;
        } else drawChar(cx, cy, s[i], color, current);
        cx += FONT_W;
    }
}

void drawRect(int x, int y, int w, int h, struct Color color, struct Size current) {
    for (int cy = y; cy < y + h; cy++) {
        for (int cx = x; cx < x + w; cx++) {
            if ((unsigned)cx < (unsigned)current.w && (unsigned)cy < (unsigned)current.h) {
                int i = (cy * current.w + cx) * 4;
                pixel[i + 0] = color.B;  /* B */
                pixel[i + 1] = color.G;  /* G */
                pixel[i + 2] = color.R;  /* R */
                pixel[i + 3] = color.A;  /* A */
            }
        }
    }
}

void drawCircle(int cx, int cy, double r, struct Color color, struct Size current) {
    unsigned int cw = (unsigned)current.w, ch = (unsigned)current.h;
    int r2 = r * r;
    if ((unsigned)(cy-r) >= ch || (unsigned)(cx-r) >= cw) return;
    double tmp = 0;
    for (int y = cy - r; y < cy + r; y++) {
        if ((unsigned)y >= ch || (unsigned)y < 0) continue;
        for (int x = cx - r; x < cx + r; x++) {
            if ((unsigned)x >= cw || (unsigned)x < 0) continue;
            if (y - cy < (tmp = sqrt(r2-pow(x - cx,2))) && y - cy > -tmp) {
                int i = (y * cw + x) * 4;
                pixel[i + 0] = color.B;  /* B */
                pixel[i + 1] = color.G;  /* G */
                pixel[i + 2] = color.R;  /* R */
                pixel[i + 3] = color.A;  /* A */
            }
        }
    }
}