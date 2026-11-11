// a minimal Wayland client in C that opens a window and draws something, 
// without relying on heavy toolkits like GTK or Qt.
// Runnable example using only wayland-client and xdg-shell protocols.
// Compile with:
// gcc wayland_app.c -o wayland_app $(pkg-config --cflags --libs wayland-client xkbcommon) -lrt -lm

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include <xkbcommon/xkbcommon.h>

// Globals for Wayland objects
struct wl_display *display = NULL;
struct wl_registry *registry = NULL;
struct wl_compositor *compositor = NULL;
struct wl_surface *surface = NULL;

// Registry listener: called when Wayland globals are advertised
static void registry_handler(void *data, struct wl_registry *registry,
                             uint32_t id, const char *interface, uint32_t version) {
    if (strcmp(interface, "wl_compositor") == 0) {
        compositor = wl_registry_bind(registry, id, &wl_compositor_interface, 4);
    }
}

static void registry_remover(void *data, struct wl_registry *registry, uint32_t id) {
    // No-op for this simple example
}

static const struct wl_registry_listener registry_listener = {
    .global = registry_handler,
    .global_remove = registry_remover
};

int main(void) {
    // Connect to Wayland display
    display = wl_display_connect(NULL);
    if (!display) {
        fprintf(stderr, "Failed to connect to Wayland display.\n");
        return EXIT_FAILURE;
    }

    // Get registry and listen for globals
    registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, NULL);
    wl_display_roundtrip(display);

    if (!compositor) {
        fprintf(stderr, "No wl_compositor found.\n");
        wl_display_disconnect(display);
        return EXIT_FAILURE;
    }

    // Create a surface
    surface = wl_compositor_create_surface(compositor);
    if (!surface) {
        fprintf(stderr, "Failed to create surface.\n");
        wl_display_disconnect(display);
        return EXIT_FAILURE;
    }

    printf("Wayland surface created. Closing in 3 seconds...\n");

    // Simple event loop: wait 3 seconds
    for (int i = 0; i < 3; i++) {
        wl_display_dispatch_pending(display);
        printf("After display i=%d\n", i);
        sleep(1);
    }

    // Cleanup
    wl_surface_destroy(surface);
    wl_compositor_destroy(compositor);
    wl_registry_destroy(registry);
    wl_display_disconnect(display);

    printf("Done.\n");
    return EXIT_SUCCESS;
}
