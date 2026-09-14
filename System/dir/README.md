# Qdir

`Qdir` is the first Q9 functional reconstruction of the OS-9/68000 `dir`
utility. The source file and OS-9 module are deliberately named `dir`; only
the delivered host/file name has the `Q` prefix.

The current C implementation supports:

- `-?` help
- `-a` include dot-prefixed entries
- `-u` unformatted one-entry-per-line output
- `-s` preserve directory enumeration order
- `-d` mark directories with `/`
- `-e` show owner, date, OS-9 attributes, descriptor sector and size
- `-r` recursive listing
- `-r=<num>` recursive listing limited to the requested depth (1--32)
- `-n` do not descend into directories during recursive listing
- `-z` and `-z=<file>` read directory paths from standard input or a file

`-x` is accepted for compatibility. Its original OS-9 meaning is tied to the
process execution-directory mechanism and is not yet reproduced by the
portable C implementation.

Multiple directory operands are accepted and listed in the order given.

The normal and extended output now follow the original `dir` layout. The
unformatted form prints complete paths, and `-n -r` treats the directory
operand as a file-like entry instead of listing its contents. As in the
original, a recursive depth of zero is rejected.

## Microware build

The program is compiled for OS-9/68000 with `xcc -O7`, linked with the shared
C runtime (`-i`), `unix.l` for `stat`, and `sys_clib.l` for the directory
functions. The linker output is intentionally named `dir` first, then copied
to `Qdir`, so the file name has the Q9 prefix while the module header remains
`dir`.

The current verified module is in `build/Qdir`.

The reproducible build is run from `c/` with:

```text
source /Volumes/SSD1TB/projects/REF/tools/macos/env/os9-toolchain.sh
REF="$MWOS_WIN" os9make REF="$MWOS_WIN"
```

The linker output is first created as `c/dir`, then the Makefile copies it to
`c/Qdir`; the checked-in delivery copy is `build/Qdir`. The latest module
verification is:

```text
Module name: dir
Module size: $1AA8 (6824 bytes)
Module CRC:  $A0182A  Good CRC
Header parity: Good parity
```
