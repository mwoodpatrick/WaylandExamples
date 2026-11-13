#define _GNU_SOURCE

// a minimal Wayland client in C that opens a surface and runs a simple event loop,
// without relying on heavy toolkits like GTK or Qt.
// Compile with:
//   gcc wayland_app.c -o wayland_app $(pkg-config --cflags --libs wayland-client xkbcommon) -lm
// or simply `make` with the provided Makefile.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include <xkbcommon/xkbcommon.h>
// Using the legacy wl_shell protocol to avoid needing generated xdg-shell headers.
static struct wl_buffer *create_shm_buffer(int width, int height, uint32_t *out_stride);

/* Global Wayland objects */
static struct wl_display *display = NULL;
static struct wl_registry *registry = NULL;
static struct wl_compositor *compositor = NULL;
static struct wl_shm *shm = NULL;
static struct wl_shell *shell = NULL;
static struct wl_surface *surface = NULL;
static struct wl_shell_surface *shell_surface = NULL;
static struct wl_seat *seat = NULL;
static struct wl_buffer *buffer = NULL;

static struct wl_pointer *pointer = NULL;

/* Registry handling */
static void registry_handler(void *data, struct wl_registry *registry,
                              uint32_t id, const char *interface, uint32_t version) {
    if (strcmp(interface, "wl_compositor") == 0) {
        compositor = wl_registry_bind(registry, id, &wl_compositor_interface, 4);
    } else if (strcmp(interface, "wl_shm") == 0) {
        shm = wl_registry_bind(registry, id, &wl_shm_interface, 1);
    } else if (strcmp(interface, "wl_shell") == 0) {
        shell = wl_registry_bind(registry, id, &wl_shell_interface, 1);
    } else if (strcmp(interface, "wl_seat") == 0) {
        seat = wl_registry_bind(registry, id, &wl_seat_interface, 5);
    }
}

static void registry_remover(void *data, struct wl_registry *registry, uint32_t id) {
    /* No‑op for this simple example */
}

static const struct wl_registry_listener registry_listener = {
    .global = registry_handler,
    .global_remove = registry_remover,
};

static void shell_surface_ping(void *data, struct wl_shell_surface *shell_surface, uint32_t serial) {
    wl_shell_surface_pong(shell_surface, serial);
}

static void shell_surface_configure(void *data, struct wl_shell_surface *shell_surface,
                                   uint32_t edges, int32_t width, int32_t height) {
    (void)data; (void)edges; // unused for this demo
    printf("Comfiguring window width=%dxheight=%d\n", width, height);
    if (width > 0 && height > 0) {
        if (buffer) {
            // Destroy the old buffer; the associated shm pool was already destroyed inside create_shm_buffer()
            wl_buffer_destroy(buffer);
            buffer = NULL;
        }
        buffer = create_shm_buffer(width, height, NULL);
        if (!buffer) {
            fprintf(stderr, "Failed to recreate buffer for resize.\n");
            return;
        }
        wl_surface_attach(surface, buffer, 0, 0);
        wl_surface_damage(surface, 0, 0, width, height);
        wl_surface_commit(surface);
    }
}

static const struct wl_shell_surface_listener shell_surface_listener = {
    .ping = shell_surface_ping,
    .configure = shell_surface_configure,
};

static void pointer_enter(void *data, struct wl_pointer *pointer,
                          uint32_t serial, struct wl_surface *surface,
                          wl_fixed_t sx, wl_fixed_t sy) {}
static void pointer_leave(void *data, struct wl_pointer *pointer,
                          uint32_t serial, struct wl_surface *surface) {}
static void pointer_motion(void *data, struct wl_pointer *pointer,
                          uint32_t time, wl_fixed_t sx, wl_fixed_t sy) {}
static void pointer_button(void *data, struct wl_pointer *pointer,
                           uint32_t serial, uint32_t time, uint32_t button,
                           uint32_t state) {
    if (state == WL_POINTER_BUTTON_STATE_PRESSED && seat) {
        wl_shell_surface_move(shell_surface, seat, serial);
    }
}
static void pointer_axis(void *data, struct wl_pointer *pointer,
                         uint32_t time, uint32_t axis, wl_fixed_t value) {}
static void pointer_frame(void *data, struct wl_pointer *pointer) {}
static const struct wl_pointer_listener pointer_listener = {
    .enter = pointer_enter,
    .leave = pointer_leave,
    .motion = pointer_motion,
    .button = pointer_button,
    .axis = pointer_axis,
    .frame = pointer_frame,
};

/* Helper to create a simple solid‑color shm buffer */
static struct wl_buffer *create_shm_buffer(int width, int height, uint32_t *out_stride) {
    int stride = width * 4; // ARGB8888
    int size = stride * height;

    int fd = memfd_create("wayland_shm", MFD_CLOEXEC);
    if (fd < 0) {
        perror("memfd_create");
        return NULL;
    }
    if (ftruncate(fd, size) < 0) {
        perror("ftruncate");
        close(fd);
        return NULL;
    }
    void *data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return NULL;
    }
    // Fill with a light‑gray colour (0xFFCCCCCC)
    uint32_t *pixels = data;
    for (int i = 0; i < width * height; ++i) {
        pixels[i] = 0xFFFF0000; // opaque red
    }
    munmap(data, size);

    struct wl_shm_pool *pool = wl_shm_create_pool(shm, fd, size);
    struct wl_buffer *buf = wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_ARGB8888);
    wl_shm_pool_destroy(pool);
    close(fd);
    if (out_stride) *out_stride = stride;
    return buf;
}

int main(void) {
    /* Connect to the Wayland display */
    display = wl_display_connect(NULL);
    if (!display) {
        fprintf(stderr, "Failed to connect to Wayland display.\n");
        return EXIT_FAILURE;
    }

    /* Get the registry and listen for globals */
    registry = wl_display_get_registry(display);
    if (wl_registry_add_listener(registry, &registry_listener, NULL) == -1) {
        fprintf(stderr, "Failed to add registry listener.\n");
        wl_display_disconnect(display);
        return EXIT_FAILURE;
    }
    wl_display_roundtrip(display);

    if (!compositor || !shm || !shell) {
        fprintf(stderr, "Missing required Wayland globals (compositor/shm/shell).\n");
        wl_display_disconnect(display);
        return EXIT_FAILURE;
    }

    /* Create a surface and a shell surface */
    surface = wl_compositor_create_surface(compositor);
    if (!surface) {
        fprintf(stderr, "Failed to create wl_surface.\n");
        wl_display_disconnect(display);
        return EXIT_FAILURE;
    }

    shell_surface = wl_shell_get_shell_surface(shell, surface);
    if (!shell_surface) {
        fprintf(stderr, "Failed to get wl_shell_surface.\n");
        wl_surface_destroy(surface);
        wl_display_disconnect(display);
        return EXIT_FAILURE;
    }
    wl_shell_surface_add_listener(shell_surface, &shell_surface_listener, NULL);
    wl_shell_surface_set_toplevel(shell_surface);
    wl_shell_surface_set_title(shell_surface, "Wayland Minimal Demo");
    /* Acquire pointer to enable moving the window via click */
    if (seat) {
        pointer = wl_seat_get_pointer(seat);
        if (pointer) {
            wl_pointer_add_listener(pointer, &pointer_listener, NULL);
        }
    }

    /* Create a simple 200×200 solid‑color buffer and attach it */
    buffer = create_shm_buffer(200, 200, NULL);
    if (!buffer) {
        fprintf(stderr, "Failed to create shm buffer.\n");
        wl_shell_surface_destroy(shell_surface);
        wl_surface_destroy(surface);
        wl_display_disconnect(display);
        return EXIT_FAILURE;
    }
    wl_surface_attach(surface, buffer, 0, 0);
    wl_surface_damage(surface, 0, 0, 200, 200);
    wl_surface_commit(surface);

    printf("Wayland window visible – press Ctrl‑C to quit.\n");

    /* Main event loop */
    while (wl_display_dispatch(display) != -1) {
        /* No extra handling needed */
    }

    /* Cleanup */
    wl_buffer_destroy(buffer);
    wl_shell_surface_destroy(shell_surface);
    wl_surface_destroy(surface);
    wl_shell_destroy(shell);
    wl_shm_destroy(shm);
    wl_compositor_destroy(compositor);
    wl_registry_destroy(registry);
    wl_display_disconnect(display);
    return EXIT_SUCCESS;
}
