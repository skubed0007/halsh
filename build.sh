#!/bin/bash

# Check if the target directory exists, if not create it
mkdir -p bin/linux
mkdir -p bin/windows

# Build the program with optimizations for Linux
cargo build --release --target=x86_64-unknown-linux-gnu

# Copy the executable to bin/linux
cp target/x86_64-unknown-linux-gnu/release/halsh bin/linux/

# Build the program with optimizations for Windows
cargo build --release --target=x86_64-pc-windows-gnu

# Copy the executable to bin/windows
cp target/x86_64-pc-windows-gnu/release/halsh.exe bin/windows/

echo "Build complete!"
