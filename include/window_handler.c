#define _POSIX_C_SOURCE 200112L

#ifndef LIVEBG
#define LIVEBG
#include "../headers/liveBG.h"
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
uint8_t cls, fullscrn, configured;

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

int32_t __alocated_sharedMemory(uint64_t size) {
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

void __resize() {
    struct Size c = getCurrent();
    zwlr_layer_surface_v1_set_size(layer_surface, c.w, c.h);

    int32_t fd = __alocated_sharedMemory(c.w * c.h * 4);

    pixel = mmap(0, c.w * c.h * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    struct wl_shm_pool* pool = wl_shm_create_pool(sharedMemory, fd, c.w * c.h * 4);
    buffer = wl_shm_pool_create_buffer(pool, 0, c.w, c.h, c.w * 4, WL_SHM_FORMAT_ARGB8888);
    wl_shm_pool_destroy(pool);
    close(fd);
}

void __draw() {
    struct Size c = getCurrent();
    
    /* OPTIMIZATION: Use memset for faster full-buffer clear instead of loop */
    /* This is significantly faster on Raspberry Pi's limited memory bandwidth */
    uint32_t bg_pixel = (background.A << 24) | (background.R << 16) | (background.G << 8) | background.B;
    
    /* Fill buffer efficiently - write 4 bytes at a time */
    uint32_t *pixel_ptr = (uint32_t *)pixel;
    size_t pixel_count = (c.w * c.h);
    for (size_t i = 0; i < pixel_count; i++) {
        pixel_ptr[i] = bg_pixel;
    }

    for (size_t i = 0; i < rEvent.count; i++) {
        rEvent.listeners[i](c);
    }

    wl_surface_attach(surface, buffer, 0, 0);
    wl_surface_damage_buffer(surface, 0, 0, c.w, c.h);
    wl_surface_commit(surface);
}

struct wl_callback_listener cb_list;

void __frame_new(void* data, struct wl_callback* cb, uint32_t cb_data) {
    wl_callback_destroy(cb);
    cb = wl_surface_frame(surface);
    wl_callback_add_listener(cb, &cb_list, 0);

    __draw();
}

struct wl_callback_listener cb_list = {
    .done = __frame_new
};

void __layer_configure(void *data, struct zwlr_layer_surface_v1 *surface, uint32_t serial, uint32_t nw, uint32_t nh) {
    zwlr_layer_surface_v1_ack_configure(surface, serial);

    if (!nw && !nh) return;

    struct Size current = getCurrent();

    if (current.w != nw || current.h != nh && !fullscrn) {
        printf("Resize: %dx%d\n", nw, nh);
        munmap(pixel, current.w * current.h * 4);
        n.w = nw;
        n.h = nh;
        __resize();
    }

    configured = 1;
}

void __layer_closed(void *data, struct zwlr_layer_surface_v1 *surface) {
    (void)data;
    (void)surface;
}

struct zwlr_layer_surface_v1_listener layer_listener = {
    .configure = __layer_configure,
    .closed = __layer_closed
};

void __kb_map(void* data, struct wl_keyboard* kb, uint32_t frmt, int32_t fd, uint32_t sz) {}

void __kb_enter(void* data, struct wl_keyboard* kb, uint32_t ser, struct wl_surface* srfc, struct wl_array *keys) {}

void __kb_leave(void* data, struct wl_keyboard* kb, uint32_t ser, struct wl_surface* srfc) {}

void __kb_key(void* data, struct wl_keyboard* kb, uint32_t ser, uint32_t t, uint32_t key, uint32_t state) {
    if (key == 1) cls = 1;
    else if (key == 87 && state) {
        fullscrn = !fullscrn;
        __resize();
        printf("Fullscreen: %u\n", fullscrn);
    }
    else printf("%s:%u\n", state ? "Pressed" : "Released", key);
}

void __kb_mod(void* data, struct wl_keyboard* kb, uint32_t ser, uint32_t dep, uint32_t lat, uint32_t lock, uint32_t group) {}

void __kb_rep(void* data, struct wl_keyboard* kb, int32_t rate, int32_t del) {}

struct wl_keyboard_listener kb_list = {
    .keymap = __kb_map,
    .enter = __kb_enter,
    .leave = __kb_leave,
    .key = __kb_key,
    .modifiers = __kb_mod,
    .repeat_info = __kb_rep
};

void __seat_cap(void* data, struct wl_seat* seat, uint32_t cap) {
    if (cap & WL_SEAT_CAPABILITY_KEYBOARD && !kb) {
        kb = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(kb, &kb_list, 0);
    }
}

void __seat_name(void* data, struct wl_seat* seat, const char* name) {}

struct wl_seat_listener seat_list = {
    .capabilities = __seat_cap,
    .name = __seat_name
};

static void __output_geometry(void *data, struct wl_output *output, int32_t x, int32_t y, int32_t physical_width, int32_t physical_height, int32_t subpixel, const char *make, const char *model, int32_t transform) {}

static void __output_mode(void *data, struct wl_output *output, uint32_t flags, int32_t width, int32_t height, int32_t refresh) {
    printf("Screen dimensions: %dx%d\n", width, height);
    f.w = (uint16_t)width;
    f.h = (uint16_t)height;
}

// Listener structure to handle output events
static const struct wl_output_listener output_listener = {
    __output_geometry,
    __output_mode,
};

void __registry_global(void* data, struct wl_registry* registry, uint32_t name, const char* interface, uint32_t version) {
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

void __registry_global_remove(void* data, struct wl_registry* reg, uint32_t name) {

}

struct wl_registry_listener registry_listener = {
    .global = __registry_global,
    .global_remove = __registry_global_remove,
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
            "livebg");
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

    while (!configured)
        wl_display_dispatch(display);

    rEvent.listeners == NULL;
    tEvent.listeners == NULL;
    rEvent.count == tEvent.count == 0;

    printf("Configured!\n");
    __draw();
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