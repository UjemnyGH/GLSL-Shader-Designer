#!/bin/bash

gcc -Ofast -Os -Wall -Wextra -Wpedantic -Werror -std=c2x -m64 -o GLSLDesigner src/*.c -I vendor/include -lGL -lglfw -lm