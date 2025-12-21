#!/bin/bash
cmake --preset=mingw64 -S . -B build -DBUILD_GODOT=OFF -DBUILD_CUTE=ON
cmake --build build

