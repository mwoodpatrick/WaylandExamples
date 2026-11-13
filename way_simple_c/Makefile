CC ?= gcc
CFLAGS :=
LIBS   := -lwayland-client -lxkbcommon -lm

all: wayland_app

wayland_app: wayland_app.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

clean:
	rm -f wayland_app
