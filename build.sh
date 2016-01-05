#!/bin/sh
gcc -Wall src/fl_manager.c src/main.c -lncurses -o bin/flmanager
echo "Done."
