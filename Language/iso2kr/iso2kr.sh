#!/bin/bash
# iso2kr - Convert ISO C to K&R format using clang-format

set -e

show_usage() {
    echo "Usage: $(basename "$0") [options] <input.c>"
    echo "Options:"
    echo "  -o <output.c>    Output file (default: overwrites input)"
    echo "  -h               Show this help message"
    exit 1
}

if [[ $# -eq 0 ]]; then
    show_usage
fi

input_file=""
output_file=""

while [[ $# -gt 0 ]]; do
    case $1 in
        -o)
            output_file="$2"
            shift 2
            ;;
        -h|--help)
            show_usage
            ;;
        *)
            input_file="$1"
            shift
            ;;
    esac
done

if [[ -z "$input_file" ]]; then
    echo "Error: No input file specified"
    show_usage
fi

if [[ ! -f "$input_file" ]]; then
    echo "Error: File '$input_file' not found"
    exit 1
fi

if ! command -v clang-format &> /dev/null; then
    echo "Error: clang-format not found. Install it with:"
    echo "  macOS: brew install clang-format"
    echo "  Linux: apt-get install clang-format"
    exit 1
fi

# Use output file if specified, otherwise use a temp file and overwrite input
if [[ -z "$output_file" ]]; then
    output_file="$input_file"
    temp_file="${input_file}.tmp"
    clang-format --style=file:"$(dirname "$0")/.clang-format" "$input_file" > "$temp_file"
    mv "$temp_file" "$output_file"
else
    clang-format --style=file:"$(dirname "$0")/.clang-format" "$input_file" > "$output_file"
fi

echo "Converted: $input_file -> $output_file"
