#!/bin/bash

# Define the compiler and flags
CC=gcc
CFLAGS="-Wall"
LIBS=""

# Check the operating system
OS=$(uname -s)

# Set the compilation flags and libraries based on the OS
if [[ "$OS" == "Linux" ]]; then
    if [[ "$(lsb_release -si)" == "Debian" ]] || [[ "$(lsb_release -si)" == "Kali" ]]; then
        LIBS=$(pkg-config --libs sdl2)
        CFLAGS+=" $(pkg-config --cflags sdl2)"
    elif [[ "$(lsb_release -si)" == "Arch" ]] || [[ "$(lsb_release -si)" == "ManjaroLinux" ]]; then
        LIBS="-lSDL2"
    fi
fi

# Compile the program
compile() {
    $CC -o fractal fractal.c $LIBS $CFLAGS
}

# Clean the project
clean() {
    rm -f fractal
}

# Main script logic
case "$1" in
    compile)
        compile
        ;;
    clean)
        clean
        ;;
    *)
        echo "Usage: ./build.sh [compile|clean]"
        exit 1
        ;;
esac

exit 0