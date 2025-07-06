#!/bin/bash

TIMESTAMPS=$(date +%s)

COMPILER="clang++"
DEBUG_FLAGS="-g"
COMPILER_FLAGS="-std=c++17"

SOURCES="src/main.cpp $(find src/utils/ -name '*.cpp') $(find src/platform/ -name '*.cpp') $(find src/renderer/ -name '*.cpp')"
SOURCES="$SOURCES $(find src/core/ -name '*.cpp') $(find src/resources/ -name '*.cpp') $(find src/math/ -name '*.cpp')"
INCLUDES="-Iinclude -Ithird_party"

DLLSRC="src/core/game.cpp src/core/input.cpp src/renderer/render_interface.cpp src/resources/assets.cpp"
DLLSRC="$DLLSRC src/math/vector.cpp"

LIBS="-luser32 -lgdi32 -lopengl32"

WARNINGS="-Wno-writable-strings -Wno-macro-redefined -Wno-c++11-narrowing"

OUTPUT="build/windows-debug/platformer.exe"
OUTDLL="build/windows-debug/game_$TIMESTAMPS.dll"

mkdir -p build/windows-debug

# Compile main executable.
$COMPILER $DEBUG_FLAGS $SOURCES $COMPILER_FLAGS -o $OUTPUT $INCLUDES $LIBS $WARNINGS

rm -f build/windows-debug/game_* # Remove old game dlls.

# Compile game dll.
$COMPILER $DEBUG_FLAGS $DLLSRC $COMPILER_FLAGS -shared -o $OUTDLL $INCLUDES $WARNINGS
mv $OUTDLL build/windows-debug/game.dll