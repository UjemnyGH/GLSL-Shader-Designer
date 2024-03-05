@echo off
gcc -Ofast -Os -Wall -Wextra -Wpedantic -Werror -std=c2x -m64 -o GLSLDesigner src/*.c -L vendor_win/lib -I vendor_win/include -lopengl32 -lglfw3 -lm -luser32 -lkernel32 -lgdi32