#ifndef LIVEBG
#define LIVEBG
#include "headers/liveBG.h"
#endif

#include <time.h>
#include <stdbool.h>

const unsigned char overridefontdata_8x16[];

/* === OPTIMIZATION: Caching for reduced CPU usage === */
static int last_clock_time = -1;
static bool clock_needs_update = true;
static bool log_needs_update = true;
static int last_quote_day = -1;
#define QUOTE_WIDTH 53
static char cached_quote[QUOTE_WIDTH + 1];

void draw_override(int x, int y, int c, struct Color color, struct Size current) {
    for (int row = 0; row < 16; row++) {
        uint8_t row_bits =
            overridefontdata_8x16[(unsigned char)c * 16 + row];

        for (int col = 0; col < 8; col++) {
            if (row_bits & 0x80) {
                int px = x + col;
                int py = y + row;

                if ((unsigned)px < (unsigned)current.w &&
                    (unsigned)py < (unsigned)current.h) {

                    int i = (py * current.w + px) * 4;
                    pixel[i + 0] = color.B;
                    pixel[i + 1] = color.G;
                    pixel[i + 2] = color.R;
                    pixel[i + 3] = color.A;
                }
            }
            row_bits <<= 1;
        }
    }
}

void draw_text_checkpoint(int x, int y, const unsigned char *s, struct Color color, struct Size current) {
    int cx = x, cy = y;
    for (size_t i = 0; s[i]; i++) {
        if (s[i] == 0x0A) {
            cy += FONT_H;
            cx = x - FONT_W;
        } else if (s[i] == 0x2D) {
            draw_override(cx, cy, 20, color, current);
        } else if (s[i] == 0x5F) {
            draw_override(cx, cy, 19, color, current);
        } else if (s[i] == 0xE2) {
            i++;
            if (s[i] == 0x95 && s[i+1] == 0xAD) {
                draw_override(cx, cy, 0, color, current);
            } else if (s[i] == 0x80 && s[i+1] == 0xBE) {
                draw_override(cx, cy, 1, color, current);
            } else if (s[i] == 0x94 && s[i+1] == 0x82) {
                drawChar(cx, cy, 0xB3, color, current);
            } else if (s[i] == 0x95 && s[i+1] == 0xAe) {
                draw_override(cx, cy, 2, color, current);
            } else if (s[i] == 0x94 && s[i+1] == 0x8C) {
                draw_override(cx, cy, 3, color, current);
            } else if (s[i] == 0x94 && s[i+1] == 0x80) {
                draw_override(cx, cy, 4, color, current);
            } else if (s[i] == 0x94 && s[i+1] == 0x90) {
                draw_override(cx, cy, 5, color, current);
            } else if (s[i] == 0x94 && s[i+1] == 0x94) {
                draw_override(cx, cy, 6, color, current);
            } else if (s[i] == 0x94 && s[i+1] == 0x98) {
                draw_override(cx, cy, 7, color, current);
            } else if (s[i] == 0x95 && s[i+1] == 0xB0) {
                draw_override(cx, cy, 8, color, current);
            } else if (s[i] == 0x95 && s[i+1] == 0xAF) {
                draw_override(cx, cy, 9, color, current);
            } else if (s[i] == 0x86 && s[i+1] == 0xBB) {
                draw_override(cx, cy, 10, color, current);
            } else if (s[i] == 0x98 && s[i+1] == 0x90) {
                draw_override(cx, cy, 11, color, current);
            } else if (s[i] == 0x80 && s[i+1] == 0x94) {
                draw_override(cx, cy, 12, color, current);
            } else if (s[i] == 0x9C && s[i+1] == 0x95) {
                draw_override(cx, cy, 13, color, current);
            } else if (s[i] == 0x99 && s[i+1] == 0xAA) {
                draw_override(cx, cy, 14, color, current);
            } else if (s[i] == 0x94 && s[i+1] == 0xAC) {
                draw_override(cx, cy, 15, color, current);
            } else if (s[i] == 0x94 && s[i+1] == 0xB4) {
                draw_override(cx, cy, 16, color, current);
            } else if (s[i] == 0x94 && s[i+1] == 0x8D) {
                draw_override(cx, cy, 17, color, current);
            } else if (s[i] == 0x94 && s[i+1] == 0x91) {
                draw_override(cx, cy, 18, color, current);
            } else {
                drawChar(cx, cy, s[--i], color, current);
                i--;
            }
            i++;
        } else drawChar(cx, cy, s[i], color, current);
        cx += FONT_W;
    }
}

const Pixel FONT_COLOR = {0xFF, 0x12, 0x7B, 0x27};

const char *digits[11][6] = {
    {
        "  ___  ",
        " / _ \\ ",
        "| | | |",
        "| | | |",
        "| |_| |",
        " \\___/ "
    },
    {
        " __ ", 
        "/_ |",
        " | |",
        " | |",
        " | |",
        " |_|"
    },
    {
        " ___  ",
        "|__ \\ ",
        "   ) |",
        "  / / ",
        " / /_ ",
        "|____|"
    },
    {
        " ____  ",
        "|___ \\ ",
        "  __) |",
        " |__ < ",
        " ___) |",
        "|____/ "
    },
    {
        " _  _   ",
        "| || |  ",
        "| || |_ ",
        "|__   _|",
        "   | |  ",
        "   |_|  "
    },
    {
        " _____ ",
        "| ____|",
        "| |__  ",
        "|___ \\ ",
        " ___) |",
        "|____/ "
    },
    {
        "   __  ",
        "  / /  ",
        " / /_  ",
        "| '_ \\ ",
        "| (_) |",
        " \\___/ "
    },
    {
        " ______",
        "|____  |",
        "    / / ",
        "   / /  ",
        "  / /   ",
        " /_/    "
    },
    {
        "  ___  ",
        " / _ \\ ",
        "| (_) |",
        " > _ < ",
        "| (_) |",
        " \\___/ "
    },
    {
        "  ___  ",
        " / _ \\ ",
        "| (_) |",
        " \\__, |",
        "   / / ",
        "  /_/  "
    },
    {
        "   ",
        "   ",
        " o ",
        "   ",
        " o ",
        "   "
    }
};

/* OPTIMIZATION: Only update clock when seconds change */
void draw_clock(struct Size current) {
    time_t now = time(NULL);
    int current_sec = (int)now % 60;
    
    if (current_sec == last_clock_time && !clock_needs_update) {
        return;  /* No visible change, skip redraw */
    }
    last_clock_time = current_sec;
    clock_needs_update = false;
    
    struct tm *tm = localtime(&now);
    
    int times[6] = {
        tm->tm_hour / 10, tm->tm_hour % 10,
        tm->tm_min / 10, tm->tm_min % 10,
        tm->tm_sec / 10, tm->tm_sec % 10
    };

    int x = current.w * 0.45, y = current.h * 0.15;
    
    for (int i = 0; i < 6; i++) {
        draw_text_checkpoint(x, y, format(296, "%s  %s  %s  %s  %s  %s  %s  %s", 
            digits[times[0]][i], digits[times[1]][i], digits[10][i], 
            digits[times[2]][i], digits[times[3]][i], digits[10][i], 
            digits[times[4]][i], digits[times[5]][i]), FONT_COLOR, current);
        y += FONT_H;
    }
}

#define MAX_LOG_LINES 200
#define LOG_LINE_LEN  128
static char log_lines[MAX_LOG_LINES][LOG_LINE_LEN];
static int log_count = 0;

static int rand_range(int min, int max) {
    return min + rand() % (max - min + 1);
}

static void rand_ip(char *out) {
    sprintf(out, "%d.%d.%d.%d",
        rand_range(10,255),
        rand_range(0,255),
        rand_range(0,255),
        rand_range(1,254));
}

static const char *levels[] = { "INFO", "WARN", "ERROR" };

static void add_log_line(void) {
    char ip[32];
    rand_ip(ip);

    char msg[LOG_LINE_LEN];
    switch (rand() % 6) {
        case 0:
            snprintf(msg, sizeof(msg),
                "Host %s is up (%.2fs latency)",
                ip, (float)rand_range(1,30)/100.0f);
            break;
        case 1:
            snprintf(msg, sizeof(msg),
                "Connection attempt from %s port %d",
                ip, rand_range(50000,60000));
            break;
        case 2:
            snprintf(msg, sizeof(msg),
                "Port %d open on %s",
                rand_range(20,9000), ip);
            break;
        case 3:
            snprintf(msg, sizeof(msg),
                "Failed password for root from %s",
                ip);
            break;
        case 4:
            snprintf(msg, sizeof(msg),
                "Blocked SYN scan from %s",
                ip);
            break;
        default:
            snprintf(msg, sizeof(msg),
                "ICMP echo request from %s",
                ip);
            break;
    }

    time_t now = time(NULL);
    struct tm *tm = localtime(&now);

    char line[LOG_LINE_LEN];
    snprintf(line, sizeof(line),
        "[%02d:%02d:%02d] %-5s %s",
        tm->tm_hour, tm->tm_min, tm->tm_sec,
        levels[rand() % 3],
        msg);

    if (log_count < MAX_LOG_LINES) {
        strcpy(log_lines[log_count++], line);
    } else {
        memmove(log_lines, log_lines + 1,
                sizeof(log_lines[0]) * (MAX_LOG_LINES - 1));
        strcpy(log_lines[MAX_LOG_LINES - 1], line);
    }
}

/* OPTIMIZATION: Increase update interval (50ms -> 200ms) for lower CPU */
void update_logs(struct Size _current) {
    static double next_time = 0;
    double now = (double)clock() / CLOCKS_PER_SEC;

    if (now > next_time) {
        add_log_line();
        log_needs_update = true;
        next_time = now + 0.2 + ((rand() % 400) / 1000.0);  /* Reduced frequency */
    }
}

/* OPTIMIZATION: Only redraw logs when they actually change */
void draw_logs(struct Size current) {
    if (!log_needs_update) {
        return;  /* Skip if no new logs */
    }
    log_needs_update = false;
    
    int y = 10;
    int LOG_VISIBLE_LINES = (current.h - y * 2) / FONT_H;
    int start = log_count > LOG_VISIBLE_LINES
              ? log_count - LOG_VISIBLE_LINES
              : 0;

    for (int i = start; i < log_count; i++) {
        draw_text_checkpoint(10, y, log_lines[i], FONT_COLOR, current);
        y += FONT_H;
    }
}


static const char *quotes[] = {
    " The cosmos is within us. We are made of star stuff. ",
    " We are a way for the cosmos to know itself. ",
    " Books permit us to voyage through time. ",
    " Something incredible waits to be known. ",
    " Imagination will carry us to other worlds. ",
    " Extraordinary claims need extraordinary proof. ",
    " Science is a way of skeptical thinking. ",
    " The universe is a pretty big place. ",
    " Vastness is bearable through love for small ones. "
};

#define QUOTE_WIDTH 53

/* OPTIMIZATION: Cache daily quote, only update once per day */
static void get_daily_quote(char *out) {
    time_t now = time(NULL);
    int days = now / (60 * 60 * 24);

    if (days != last_quote_day) {
        const char *q = quotes[days % (sizeof(quotes)/sizeof(quotes[0]))];
        snprintf(cached_quote, sizeof(cached_quote), "%-53s", q);
        last_quote_day = days;
    }
    strcpy(out, cached_quote);
}

char black_hole[] = "╭‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾╮\n"
"│   <   >   ↻                                                       ☐  —  ✕   │\n"
"│-----------------------------------------------------------------------------│\n"
"│  https://quoteoftheday.com                                                  │\n"
"│-----------------------------------------------------------------------------│\n"
"│                                                                             │\n"
"│                                                                             │\n"
"│           ┌─────────────────────────────────────────────────────┐           │\n"
"│           │                                                     │           │\n"
"│           │                  Quote Of The Day:                  │           │\n"
"│           │                                                     │           │\n"
"│           │%s│           │\n"
"│           │                                                     │           │‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾╮\n"
"│           │                   - Carl Sagan                      │           │                                            ☐  —  ✕    │\n"
"│           │                                                     │           │-------------------------------------------------------│\n"
"│           └─────────────────────────────────────────────────────┘           │                                                       │\n"
"│                                                                             │-------------------------------------------------------│\n"
"╰_____________________________________________________________________________╯                                                       │\n"
"                                                           │                                                                          │\n"
"                                                           │               ┍───────────────────────────────────────────┑              │\n"
"                                                           │               │                                           │              │\n"
"                                                           │               │                                           │              │\n"
"                                                           │               │  Never Let Me Down Again - 2006 Remaster  │              │\n"
"                                                           │               │                                           │              │\n"
"                                                           │               │        ♪      Depeche Mode      ♪         │              │\n"
"                                                           │               │                                           │              │\n"
"                                                           │               │    |────────────────────────────────|     │              │\n"
"                                                           │               │                                           │              │\n"
"                                                           │               │ ┍───────┬──────┬────────┬──────┬────────┑ │              │\n"
"                                                           │               │ │ SHUFF │  <<  │   ||   │  >>  │  REPT  │ │              │\n"
"                                                           │               └─└───────┴──────┴────────┴──────┴────────┘─┘              │\n"
"                                                           │                                                                          │\n"
"                                                           ╰__________________________________________________________________________╯";

/* OPTIMIZATION: Static buffer to avoid malloc in render loop */
static char black_hole_buffer[sizeof(black_hole) + QUOTE_WIDTH + 1];
static bool black_hole_initialized = false;

void draw_black_hole(struct Size current) {
    if (!black_hole_initialized) {
        char quote[QUOTE_WIDTH + 1];
        get_daily_quote(quote);
        snprintf(black_hole_buffer, sizeof(black_hole_buffer), black_hole, quote);
        black_hole_initialized = true;
    }
    
    /* Only rebuild if quote changed (once per day) */
    if (last_quote_day != (int)(time(NULL) / (60 * 60 * 24))) {
        char quote[QUOTE_WIDTH + 1];
        get_daily_quote(quote);
        snprintf(black_hole_buffer, sizeof(black_hole_buffer), black_hole, quote);
    }
    
    draw_text_checkpoint(current.w * 0.35, current.h * 0.35, black_hole_buffer, FONT_COLOR, current);
}

int main() {
    appendRenderListener(update_logs);
    appendRenderListener(draw_clock);
    appendRenderListener(draw_black_hole);
    appendRenderListener(draw_logs);

    return runWindow();
}

const unsigned char overridefontdata_8x16[] = {
	/* 0 0x00 '╭' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x01, /* 00000001 */
	0x03, /* 00000011 */
	0x06, /* 00000110 */
	0x0C, /* 00001100 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	/* 1 0x01 '‾' */
	0x00, /* 00000000 */
	0xFF, /* 11111111 */
	0xFF, /* 11111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	/* 2 0x02 '╮' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x80, /* 10000000 */
	0xC0, /* 11000000 */
	0x60, /* 01100000 */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	/* 3 0x03 '┌' */
	0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
    0x1F, /* 00011111 */
    0x1F, /* 00011111 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
	0x18, /* 00011000 */
	/* 4 0x04 '─' */
	0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
    0xFF, /* 11111111 */
    0xFF, /* 11111111 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
	/* 5 0x05 '┐' */
	0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
    0xF8, /* 11111000 */
    0xF8, /* 11111000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
	0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
	/* 6 0x06 '└' */
	0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
    0x0F, /* 00001111 */
    0x07, /* 00000111 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
	0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
	/* 7 0x07 '┘' */
	0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
    0xF0, /* 11110000 */
    0xE0, /* 11100000 */
    0x00, /* 00000000 */
	0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
	/* 8 0x08 '╰' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x0C, /* 00001100 */
	0x06, /* 00000110 */
	0x03, /* 00000011 */
	0x01, /* 00000001 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	/* 9 0x09 '╯' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x60, /* 01100000 */
	0xC0, /* 11000000 */
	0x80, /* 10000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	/* 10 0x0A '↻' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x74, /* 01110100 */
	0x32, /* 00110010 */
	0x52, /* 01010010 */
	0x42, /* 01000010 */
	0x42, /* 01000010 */
	0x3C, /* 00111100 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	/* 11 0x0B '☐' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xFF, /* 11111111 */
	0x81, /* 10000001 */
	0x81, /* 10000001 */
	0x81, /* 10000001 */
	0x81, /* 10000001 */
	0x81, /* 10000001 */
	0x81, /* 10000001 */
	0xFF, /* 11111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	/* 12 0x0C '—' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xFF, /* 11111111 */
	0xFF, /* 11111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	/* 13 0x0D '✕' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xC3, /* 11000011 */
	0xE7, /* 11100111 */
	0x7E, /* 01111110 */
	0x3C, /* 00111100 */
	0x3C, /* 00111100 */
	0x7E, /* 01111110 */
	0xE7, /* 11100111 */
	0xC3, /* 11000011 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	/* 14 0x0E '♪' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x08, /* 00001000 */
	0x0C, /* 00001100 */
	0x0A, /* 00001010 */
	0x0A, /* 00001010 */
	0x08, /* 00001000 */
	0x08, /* 00001000 */
	0x08, /* 00001000 */
	0x38, /* 00111000 */
	0x78, /* 01111000 */
	0x70, /* 01110000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	/* 15 0x0F '┬' */
	0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
    0xFF, /* 11111111 */
    0xFF, /* 11111111 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
	/* 16 0x10 '┴' */
	0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0xFF, /* 11111111 */
    0xFF, /* 11111111 */
	0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	/* 17 0x11 '┍' */
	0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x07, /* 00000111 */
    0x0F, /* 00001111 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
	/* 18 0x12 '┑' */
	0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
    0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xE0, /* 11100000 */
    0xF0, /* 11110000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    0x18, /* 00011000 */
    /* 19 0x13 '_' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xFF, /* 11111111 */
	0xFF, /* 11111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
    /* 20 0x14 '-' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x3C, /* 00111100 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
};