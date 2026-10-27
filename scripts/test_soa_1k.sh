#!/bin/bash

BUILD_DIR="out/build/clang-tidy/soa/Release"
INPUT_DIR="input"

WIDTH=1000
HEIGHT=1000
NUM_ITER=500
DELTA=0.25

# 1K
if [ -d "$INPUT_DIR" ]; then
    mv "$INPUT_DIR/input1K.txt" "$INPUT_DIR/input.txt"
    if [ -d "$BUILD_DIR" ]; then
        perf stat "$BUILD_DIR/grav-soa" $NUM_ITER $WIDTH $HEIGHT $DELTA
    else
        echo "Not found: $BUILD_DIR"
    fi
    mv "$INPUT_DIR/input.txt" "$INPUT_DIR/input1K.txt"
else
    echo "Not found: $INPUT_DIR"
fi

# 10K
