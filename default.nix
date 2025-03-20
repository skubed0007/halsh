{pkgs ? import <nixpkgs> {}}:
pkgs.stdenv.mkDerivation rec {
  pname = "halsh";
  version = "0.55";

  src = ./.; # Your source code is in the current directory

  # Build dependencies – adjust these if you need additional libraries
  buildInputs = [
    pkgs.clang
    pkgs.readline
    pkgs.ncurses
    pkgs.editline
    pkgs.glibc
  ];

  # Build phase: compile your main.c into an executable named "halsh"
  buildPhase = ''

  clang -std=gnu11 -D__gnuc_va_list=__builtin_va_list \
    -O3 -flto -march=native -mtune=native -ffast-math -fno-math-errno \
    -funsafe-math-optimizations -fno-common -fvisibility=hidden \
    -fdata-sections -ffunction-sections -finline-functions \
    -funroll-loops -flto=full -fomit-frame-pointer \
    -o halsh main.c config_parser.c \
    -lreadline -lncurses -pthread -ledit


  '';

  # Install phase: copy the executable into $out/bin
  installPhase = ''
    mkdir -p $out/bin
    cp halsh $out/bin/
  '';

  meta = with pkgs.lib; {
    description = "Halsh - A custom minimal shell";
  license = { name = "Joay License 1.5"; file = ./LICENSE.md; };
    maintainers = ["Joy False"];
    platforms = platforms.unix;
  };
}
