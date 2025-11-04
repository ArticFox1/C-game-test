#!/bin/bash

libs="-luser32 -lopengl32"
warnings="-Wno-writable-strings -Wno-format-security"
includes="-IGL_lib -IGL_lib/Includes"

clang++ $includes -g src/main.cpp -otest.exe $libs $warnings