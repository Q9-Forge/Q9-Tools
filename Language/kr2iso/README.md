# kr2iso - K&R to ISO C Format Converter

Converts C code from Kernighan & Ritchie (K&R) style to ISO C (modern ANSI-C style).

## Features

- Converts function declarations from K&R style to ISO style
- Preserves all code logic and comments
- Uses clang-format with LLVM (modern ISO) configuration

## Installation

```bash
# Requires clang-format
# macOS: brew install clang-format
# Linux: apt-get install clang-format
# Windows: Download from LLVM releases
```

## Usage

```bash
./kr2iso.sh input.c
./kr2iso.sh input.c -o output.c
./kr2iso.sh *.c  # Batch conversion
```

## ISO C Style

ISO C style (LLVM format):
- Opening brace on new line for functions
- 2-space indentation
- Modern function declarations with types

## Example

K&R Style:
```c
int
add(a, b)
    int a;
    int b;
{
    return a + b;
}
```

ISO Style:
```c
int add(int a, int b) {
    return a + b;
}
```
