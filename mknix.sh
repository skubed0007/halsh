#!/usr/bin/env bash

if [ ! -f main.c ]; then
    echo "Error: main.c not found in the current directory."
    exit 1
fi

if [ ! -f config_parser.c ]; then
    echo "Error: config_parser.c not found in the current directory."
    exit 1
fi

echo "Compiling main.c and config_parser.c into halsh..."

clang -std=gnu11 -D__gnuc_va_list=__builtin_va_list \
    -O3 -flto -march=native -mtune=native -ffast-math -fno-math-errno \
    -funsafe-math-optimizations -fno-common -fvisibility=hidden \
    -fdata-sections -ffunction-sections -finline-functions \
    -funroll-loops -flto=full -fomit-frame-pointer \
    -o halsh main.c config_parser.c \
    -lreadline -lncurses -pthread -ledit

if [ $? -eq 0 ]; then
    echo "Compilation successful. Executable 'halsh' created."
else
    echo "Compilation failed."
    exit 1
fi
