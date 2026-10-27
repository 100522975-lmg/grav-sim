#!/bin/bash

BUILD_DIR="out/build/clang-tidy/aos/Release"
INPUT_DIR="input"

WIDTH=1000
HEIGHT=1000
NUM_ITER=20
DELTA=0.1

# 10K
if [-d "$INPUT_DIR"]; then
    mv "$INPUT_DIR/input10K.txt" "$INPUT_DIR/input.txt"
    if [-d "$BUILD_DIR"]; then
        perf stat "$BUILD_DIR/grav-soa" $NUM_ITER $WIDTH $HEIGHT $DELTA
    else
        echo "Not found: $BUILD_DIR"
    fi
    mv "$INPUT_DIR/input.txt" "$INPUT_DIR/input10K.txt"
else
    echo "Not found: $INPUT_DIR"
fi
