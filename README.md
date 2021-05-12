# Minimal Breaking Example for VTerm Resize

This repo provides a minimal breaking example for [bug
1927320](https://bugs.launchpad.net/libvterm/+bug/1927320) in libvterm.
Calling `vterm_screen_get_text` in a damage callback during resize
segfaults when the number of rows is decreasing and columns increasing. If
the number of columns is increasing, a rect is damaged with the old number
of rows after the screen buffer is re-alloced. Since the number of rows is
increasing in this case, that means the screen buffer size has already
been reduced and accessing cells beyond the new number of rows fails.
See log in `valgrind.log` and output in `out.log`.

The logs also demonstrate that all other deltas of resizing (decreasing rows
while increasing columns, decreasing rows while fixing columns, etc.)
support the same usage of `vterm_screen_get_text` in a damage callback.

# Replicating

To replicate run:
```
make && valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --log-file=valgrind.log \
    --keep-debuginfo=yes \
    ./main > out.log
```

The included `valgrind.log` file includes file names and line numbers from
the [libvterm library here](https://github.com/neovim/libvterm) which was
built and installed with flags `CFLAGS=-g3 LDFLAGS=-g` to include debug
info for valgrind.
