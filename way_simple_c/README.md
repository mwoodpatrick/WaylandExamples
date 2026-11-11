Wayland minimal client example
================================

A tiny C program that connects to a Wayland compositor, creates a surface, and stays alive for a few seconds.  
The code deliberately avoids heavyweight toolkits (GTK, Qt) and uses only the core Wayland client library and xkbcommon.

---

### Prerequisites
- Wayland development packages (`wayland-client`, `wayland-protocols`)
- `xkbcommon` development headers
- A C compiler (e.g., `gcc`)
- `pkg-config` to resolve build flags

On Debian/Ubuntu you can install the required packages with:

```sh
sudo apt-get install build-essential pkg-config libwayland-dev libwayland-client0 libxkbcommon-dev
```

---

### Build

```sh
gcc wayland_app.c -o wayland_app $(pkg-config --cflags --libs wayland-client xkbcommon) -lrt -lm
```

The command is also shown as a comment at the top of `wayland_app.c`.

---

### Run

```sh
./wayland_app
```

The program will:
1. Connect to the Wayland display server.
2. Retrieve the `wl_compositor` global.
3. Create a surface.
4. Print a message and wait 3 seconds while processing any pending Wayland events.
5. Clean up and exit.

If the compositor does not expose `wl_compositor`, the program exits with an error message.

---

### What the example demonstrates
- Basic Wayland connection workflow (`wl_display_connect`, `wl_display_get_registry`).
- Registry handling to bind required globals.
- Surface creation and destruction.
- Simple event loop using `wl_display_dispatch_pending`.

---

### Extending the example
- Add listeners for `wl_surface` or `xdg_wm_base` to manage window properties.
- Render graphics via `wl_shm`
- Implement keyboard handling using `xkbcommon`.
- Add XDG‑shell support, 


---

### License
Public domain / MIT (choose whichever applies to your project).
