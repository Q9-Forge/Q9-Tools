# iso2kr - ISO C to K&R Format Converter

Converts C code from ISO C (modern ANSI-C style) to Kernighan & Ritchie (K&R) style.

## Features

- Converts function declarations from ISO style to K&R style
- Preserves all code logic and comments
- Uses clang-format with K&R configuration

## Installation

```bash
# Requires clang-format
# macOS: brew install clang-format
# Linux: apt-get install clang-format
# Windows: Download from LLVM releases
```

## Usage

```bash
./iso2kr.sh input.c
./iso2kr.sh input.c -o output.c
./iso2kr.sh *.c  # Batch conversion
```

## K&R Style

K&R style uses classic C formatting:
- Opening brace on same line for functions
- 4-space indentation
- Function parameters on separate lines

## Example

ISO Style:
```c
int add(int a, int b) {
    return a + b;
}
```

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
