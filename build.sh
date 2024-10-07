#!/bin/bash

BUILD_DIR="build"
OUT_DIR="out"


flex lexer.l
bison -d -t parser.y

if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
    echo "Folder created: $BUILD_DIR"
else
    echo "Folder already exists: $BUILD_DIR"
fi



gcc -c -o "${BUILD_DIR}/error.o" error.c
echo "gcc -c -o error.o error.c"

gcc -c -o "${BUILD_DIR}/node.o" node.c
echo "gcc -c -o "${BUILD_DIR}/node.o" node.c"

echo "gcc -c -o "${BUILD_DIR}/lex.yy.o" lex.yy.c"
gcc -c -o "${BUILD_DIR}/lex.yy.o" lex.yy.c

echo "gcc -c -o "${BUILD_DIR}/parser.tab.o" parser.tab.c"
gcc -c -o "${BUILD_DIR}/parser.tab.o" parser.tab.c

echo "gcc -c -o "${BUILD_DIR}/slice.o" slice.c"
gcc -c -o "${BUILD_DIR}/slice.o" slice.c

echo "gcc -c -o "${BUILD_DIR}/asmGeneration.o" asmGeneration.c" 
gcc -c -o "${BUILD_DIR}/asmGeneration.o" asmGeneration.c

echo "gcc -c -o "${BUILD_DIR}/streamGraph.o" streamGraph.c" 
gcc -c -o "${BUILD_DIR}/streamGraph.o" streamGraph.c

echo "gcc -c -o "${BUILD_DIR}/main.o" main.c"
gcc -c -o "${BUILD_DIR}/main.o" main.c


if [ ! -d "$OUT_DIR" ]; then
    mkdir "$OUT_DIR"
    echo "Folder created: $OUT_DIR"
else
    echo "Folder already exists: $OUT_DIR"
fi

echo "Build object files..."
gcc -o "${OUT_DIR}/parser" "${BUILD_DIR}/error.o" "${BUILD_DIR}/lex.yy.o" "${BUILD_DIR}/main.o" "${BUILD_DIR}/node.o" "${BUILD_DIR}/parser.tab.o" "${BUILD_DIR}/streamGraph.o" "${BUILD_DIR}/slice.o" "${BUILD_DIR}/asmGeneration.o"

sleep 15