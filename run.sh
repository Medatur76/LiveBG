#!/bin/sh
gcc -o program ./*.c ./include/*.c  -lwayland-client -lm
./program
clear