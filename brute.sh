#!/bin/bash

BIN='/tmp/brute_b2b74b2c345b8dd2feda2720e3ae00c0'
SRC=brute.cpp
CPP=g++
OPTS=(-std=c++20 -Ofast)
LIBS=(-lcrypto++ -lpthread)

rm -rf "$BIN"
if [ "$1" == -d ]; then
  "$CPP" -Og -g "$OPTS" "$SRC" -o "$BIN" "$LIBS"
  valgrind \
    --tool=memcheck \
    --leak-check=full \
    --leak-resolution=high \
    --leak-check-heuristics=all \
    --show-leak-kinds=all \
    --track-origins=yes \
    "$BIN"
else
  "$CPP" "$OPTS" "$SRC" -o "$BIN" "$LIBS"
  "$BIN"
fi
