clang -O3 -flto -march=native -mtune=native -ffast-math -fno-math-errno \
    -funsafe-math-optimizations -fno-common -fvisibility=hidden \
    -fdata-sections -ffunction-sections -finline-functions \
    -funroll-loops -flto=full -fomit-frame-pointer \
    -o halsh main.c \
    -lreadline -lncurses -pthread

