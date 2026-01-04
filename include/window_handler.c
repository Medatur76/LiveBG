#define _POSIX_C_SOURCE 200112L

#ifndef WINDOW_HANDLER
#define WINDOW_HANDLER
#include "../headers/window_handler.h"
#endif

Pixel background = {0xFF,0x00,0x00,0x00};

struct RenderEvent {
    RenderEventListener *listeners;
    size_t count;
} rEvent;

struct TickEvent {
    TickEventListener *listeners;
    size_t count;
} tEvent;

struct wl_compositor* compositor;
struct wl_surface* surface;
struct wl_buffer* buffer;
struct wl_shm* sharedMemory;
struct zwlr_layer_shell_v1 *layer_shell;
struct zwlr_layer_surface_v1 *layer_surface;
struct wl_seat* seat;
struct wl_keyboard* kb;
uint8_t* pixel;
struct Size n = {1920, 1080}, f = {1920, 1080};
int configured = 0;
uint8_t cls, fullscrn;

struct Size getCurrent() {
    return fullscrn ? f : n;
}

void appendRenderListener(RenderEventListener listener) {
    rEvent.listeners = realloc(rEvent.listeners, ++rEvent.count * sizeof(RenderEventListener));
    rEvent.listeners[rEvent.count - 1] = listener;
}

void appendTickListener(TickEventListener listener) {
    tEvent.listeners = realloc(tEvent.listeners, ++tEvent.count * sizeof(TickEventListener));
    tEvent.listeners[tEvent.count - 1] = listener;
}

char* format(size_t maxlen, const char *__restrict format, ...) {
    char *out = malloc(maxlen);

    va_list ap;
    va_start(ap, format);
    vsnprintf(out, maxlen, format, ap);

    va_end(ap);

    return out;
}

int32_t alocated_sharedMemory(uint64_t size) {
    int8_t name[8];
    name[0] = '/';
    name[7] = 0;
    for (uint8_t i = 1; i < 6; i++)
        name[i] = (rand() & 23) + 97;
    int32_t fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);
    shm_unlink(name);
    ftruncate(fd, size);
    
    return fd;
}

void resize() {
    struct Size c = getCurrent();
    zwlr_layer_surface_v1_set_size(layer_surface, c.w, c.h);

    int32_t fd = alocated_sharedMemory(c.w * c.h * 4);

    pixel = mmap(0, c.w * c.h * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    struct wl_shm_pool* pool = wl_shm_create_pool(sharedMemory, fd, c.w * c.h * 4);
    buffer = wl_shm_pool_create_buffer(pool, 0, c.w, c.h, c.w * 4, WL_SHM_FORMAT_ARGB8888);
    wl_shm_pool_destroy(pool);
    close(fd);
}

void draw_char(int x, int y, char c, struct Color color, struct Size current) {
    const int fw = 8;   /* expected: 8 */
    const int fh = 16;  /* expected: 16 */

    /* Clamp to available glyphs */
    if (c >= 256)
        return;
    
    for (int row = 0; row < fh; row++) {
        /* One byte = one horizontal row of 8 pixels */
        uint8_t row_bits =
            fontdata_8x16[(unsigned char)c * fh + row];

        for (int col = 0; col < fw; col++) {
            /* MSB-first: leftmost pixel is bit 7 */
            if (row_bits & 0x80) {
                int px = x + col;
                int py = y + row;

                if ((unsigned)px < (unsigned)current.w &&
                    (unsigned)py < (unsigned)current.h) {

                    int i = (py * current.w + px) * 4;
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

void draw_text(int x, int y, const unsigned char *s, struct Color color, struct Size current) {
    int cx = x, cy = y;
    for (size_t i = 0; s[i]; i++) {
        if (s[i] == 0x0A) {
            cy += FONT_H;
            cx = x - FONT_W;
        } else draw_char(cx, cy, s[i], color, current);
        cx += FONT_W;
    }
}

void draw() {
    struct Size c = getCurrent();
    size_t pixel_count = c.w * c.h * 4;
    for (size_t i = 0; i < pixel_count; i+=4) {
        pixel[i] = background.B;
        pixel[i+1] = background.G;
        pixel[i+2] = background.R;
        pixel[i+3] = background.A;
    }


    /*update_logs();
    draw_logs(c);
    draw_clock(c.w * 0.45, c.h * 0.15, c);
    draw_black_hole(c);*/

    for (size_t i = 0; i < rEvent.count; i++) {
        rEvent.listeners[i](c);
    }

    wl_surface_attach(surface, buffer, 0, 0);
    wl_surface_damage_buffer(surface, 0, 0, c.w, c.h);
    wl_surface_commit(surface);
}

struct wl_callback_listener cb_list;

void frame_new(void* data, struct wl_callback* cb, uint32_t cb_data) {
    wl_callback_destroy(cb);
    cb = wl_surface_frame(surface);
    wl_callback_add_listener(cb, &cb_list, 0);

    draw();
}

struct wl_callback_listener cb_list = {
    .done = frame_new
};

void layer_configure(void *data, struct zwlr_layer_surface_v1 *surface, uint32_t serial, uint32_t nw, uint32_t nh) {
    zwlr_layer_surface_v1_ack_configure(surface, serial);

    if (!nw && !nh) return;

    struct Size current = getCurrent();

    if (current.w != nw || current.h != nh && !fullscrn) {
        printf("Resize: %dx%d\n", nw, nh);
        munmap(pixel, current.w * current.h * 4);
        n.w = nw;
        n.h = nh;
        resize();
    }

    configured = 1;
}

void layer_closed(void *data, struct zwlr_layer_surface_v1 *surface) {
    (void)data;
    (void)surface;
}

struct zwlr_layer_surface_v1_listener layer_listener = {
    .configure = layer_configure,
    .closed = layer_closed
};

void kb_map(void* data, struct wl_keyboard* kb, uint32_t frmt, int32_t fd, uint32_t sz) {}

void kb_enter(void* data, struct wl_keyboard* kb, uint32_t ser, struct wl_surface* srfc, struct wl_array *keys) {}

void kb_leave(void* data, struct wl_keyboard* kb, uint32_t ser, struct wl_surface* srfc) {}

void kb_key(void* data, struct wl_keyboard* kb, uint32_t ser, uint32_t t, uint32_t key, uint32_t state) {
    if (key == 1) cls = 1;
    else if (key == 87 && state) {
        fullscrn = !fullscrn;
        resize();
        printf("Fullscreen: %u\n", fullscrn);
    }
    else printf("%s:%u\n", state ? "Pressed" : "Released", key);
}

void kb_mod(void* data, struct wl_keyboard* kb, uint32_t ser, uint32_t dep, uint32_t lat, uint32_t lock, uint32_t group) {}

void kb_rep(void* data, struct wl_keyboard* kb, int32_t rate, int32_t del) {}

struct wl_keyboard_listener kb_list = {
    .keymap = kb_map,
    .enter = kb_enter,
    .leave = kb_leave,
    .key = kb_key,
    .modifiers = kb_mod,
    .repeat_info = kb_rep
};

void seat_cap(void* data, struct wl_seat* seat, uint32_t cap) {
    if (cap & WL_SEAT_CAPABILITY_KEYBOARD && !kb) {
        kb = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(kb, &kb_list, 0);
    }
}

void seat_name(void* data, struct wl_seat* seat, const char* name) {}

struct wl_seat_listener seat_list = {
    .capabilities = seat_cap,
    .name = seat_name
};

static void output_geometry(void *data, struct wl_output *output, int32_t x, int32_t y, int32_t physical_width, int32_t physical_height, int32_t subpixel, const char *make, const char *model, int32_t transform) {}

static void output_mode(void *data, struct wl_output *output, uint32_t flags, int32_t width, int32_t height, int32_t refresh) {
    printf("Screen dimensions: %dx%d\n", width, height);
    f.w = (uint16_t)width;
    f.h = (uint16_t)height;
}

// Listener structure to handle output events
static const struct wl_output_listener output_listener = {
    output_geometry,
    output_mode,
};

void registry_global(void* data, struct wl_registry* registry, uint32_t name, const char* interface, uint32_t version) {
    if (!strcmp(interface, wl_output_interface.name)) {
        struct wl_output *output = wl_registry_bind(registry, name, &wl_output_interface, 1);
        wl_output_add_listener(output, &output_listener, 0);
    } else if (!strcmp(interface, wl_compositor_interface.name)) {
        compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 4);
    } else if (!strcmp(interface, wl_shm_interface.name)) {
        sharedMemory = wl_registry_bind(registry, name, &wl_shm_interface, 1);
    } else if (!strcmp(interface, zwlr_layer_shell_v1_interface.name)) {
        layer_shell = wl_registry_bind(registry, name, &zwlr_layer_shell_v1_interface, 1);
    } else if (!strcmp(interface, wl_seat_interface.name)) {
        seat = wl_registry_bind(registry, name, &wl_seat_interface, 1);
        wl_seat_add_listener(seat, &seat_list, 0);
    }
}

void registry_global_remove(void* data, struct wl_registry* reg, uint32_t name) {

}

struct wl_registry_listener registry_listener = {
    .global = registry_global,
    .global_remove = registry_global_remove,
};

int runWindow() {
    struct wl_display* display = wl_display_connect(0);
    if (!display)
        return -1;

    struct wl_registry* registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, 0);
    wl_display_roundtrip(display);

    surface = wl_compositor_create_surface(compositor);
    struct wl_callback* cb = wl_surface_frame(surface);
    wl_callback_add_listener(cb, &cb_list, 0);

    layer_surface =
        zwlr_layer_shell_v1_get_layer_surface(
            layer_shell,
            surface,
            NULL,
            ZWLR_LAYER_SHELL_V1_LAYER_BOTTOM,
            "[name-here]");
    zwlr_layer_surface_v1_add_listener(layer_surface,
                                       &layer_listener,
                                       NULL);
    zwlr_layer_surface_v1_set_size(layer_surface, 1920, 1044);

    zwlr_layer_surface_v1_set_anchor(
        layer_surface,
        ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM |
        ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT |
        ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT);

    zwlr_layer_surface_v1_set_exclusive_zone(layer_surface, -1);
    wl_surface_commit(surface);

    /* Wait for initial configure */
    while (!configured)
        wl_display_dispatch(display);

    rEvent.listeners == NULL;
    tEvent.listeners == NULL;
    rEvent.count == tEvent.count == 0;

    printf("Configured!\n");
    draw();
    zwlr_layer_surface_v1_set_keyboard_interactivity(layer_surface, ZWLR_LAYER_SURFACE_V1_KEYBOARD_INTERACTIVITY_ON_DEMAND);
    
    while (wl_display_dispatch(display))
        if (cls) break;

    if (kb) wl_keyboard_destroy(kb);
    wl_seat_release(seat);
    if (buffer) wl_buffer_destroy(buffer);
    zwlr_layer_surface_v1_destroy(layer_surface);
    zwlr_layer_shell_v1_destroy(layer_shell);
    wl_surface_destroy(surface);
    wl_display_disconnect(display);

    return 0;
}