Wayland minimal client example

## Table of Contents
- [Prerequisites](#prerequisites)
- [Build](#build)
  - [Plain GCC](#plain-gcc)
  - [Makefile](#makefile)
  - [Nix](#nix)
- [Run](#run)
- [What the example demonstrates](#what-the-example-demonstrates)
- [Extending the example](#extending-the-example)
- [Generating a compilation database](#generating-a-compilation-database)
- [License](#license)
===============================

A tiny C program that connects to a Wayland compositor, opens a surface, 
and runs a simple event loop. The code deliberately avoids 
heavyweight toolkits (GTK, Qt) and uses only the core Wayland client library 
and **xkbcommon**, XDG shell, and shared memory for drawing.

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

#### Using plain GCC (as shown in the original example)
```sh
gcc wayland_app.c -o wayland_app $(pkg-config --cflags --libs wayland-client xkbcommon) -lm
```
> **Note:** `-lrt` is no longer required on modern glibc, so it has been removed.

#### Using the provided Makefile
```sh
make            # compiles `wayland_app`
make clean      # removes the binary and temporary files
```

#### Using Nix (if you have the Nix package manager)
```sh
nix develop      # drops you into a shell with all build dependencies
make
```

The exact compilation command is also shown as a comment at the top of `wayland_app.c`.
---

### Run

```sh
./wayland_app
```

The program will:
1. Connect to the Wayland display server.
2. Retrieve the `wl_compositor` global.
3. Create a surface.
4. Print a message and run an event loop that processes pending Wayland events.
5. Clean up and exit when the user terminates the program (Ctrl‑C).

If the compositor does not expose `wl_compositor`, the program exits with an error message.
---

### What the example demonstrates
- Basic Wayland connection workflow (`wl_display_connect`, `wl_display_get_registry`).
- Registry handling to bind required globals.
- Surface creation and destruction.
- Uses the XDG‑shell protocol 
- Uses `wl_shm` for simple drawing.
- Simple event loop using `wl_display_dispatch_pending`.
---

### Extending the example
- Add listeners for `wl_surface` or `xdg_wm_base` to manage window properties.
- Render graphics via `wl_shm`.
- Implement keyboard handling using **xkbcommon**.
- Add full XDG‑shell support (e.g., create an `xdg_surface` and `xdg_toplevel`).
---

### Generating a compilation database
If you use an IDE that understands `compile_commands.json`, you can generate it with:
```sh
bear -- make
```
The resulting `compile_commands.json` will be placed in the project root.
---

### License

This project is released under the **MIT License**. See the `LICENSE` file for the full text.
