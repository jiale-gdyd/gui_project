#!/bin/bash

SOURCES=xdg-shell-protocol.c
HEADERS=xdg-shell-client-protocol.h

export WAYLAND_FLAGS=$(pkg-config wayland-client --cflags --libs)
export WAYLAND_SCANNER=$(pkg-config --variable=wayland_scanner wayland-scanner)
export WAYLAND_PROTOCOLS_DIR=$(pkg-config wayland-protocols --variable=pkgdatadir)

export XDG_SHELL_PROTOCOL=${WAYLAND_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml

${WAYLAND_SCANNER} client-header ${XDG_SHELL_PROTOCOL} xdg-shell-client-protocol.h

${WAYLAND_SCANNER} private-code ${XDG_SHELL_PROTOCOL} xdg-shell-protocol.c
