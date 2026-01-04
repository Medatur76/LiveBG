#!/bin/sh
gcc -o program ./*.c ./include/*.c  -lwayland-client
./program
clear