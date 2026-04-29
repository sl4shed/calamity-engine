#!/bin/bash
set -e
echo "Make sure to have emsdk activated!"
EXAMPLES_DIR="examples"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
CLEAN=false
WEB_ONLY=false

# parse args
for arg in "$@"; do
    case $arg in
        --clean)
            CLEAN=true
            shift
            ;;
        --web-only)
            WEB_ONLY=true
            shift
            ;;
    esac
done

for example in "$EXAMPLES_DIR"/*/; do
    [ -d "$example" ] || continue
    echo "==> Building: $example"
    cd "$example"

    # optionally clean build
    for build_dir in build build-web build-psp; do
        if [ "$CLEAN" = true ]; then
            rm -rf "$build_dir"
        fi
        mkdir -p "$build_dir"
    done

    if [ "$WEB_ONLY" = false ]; then
        # native build
        cd build
        echo " -> building native..."
        {
            cmake -G Ninja -DBUILD_TARGET=NATIVE -DCMAKE_BUILD_TYPE=Release ..
            ninja
        } > /dev/null 2>&1 || echo " -> native build FAILED"
        cd ..
    fi

    # emscripten build
    cd build-web
    echo " -> building emscripten..."
    {
        emcmake cmake -DBUILD_TARGET=EMSCRIPTEN -DCMAKE_BUILD_TYPE=Release -DSDLTTF_VENDORED=ON -DCALAMITY_VENDORED=ON ..
        emmake make
    } > /dev/null 2>&1 || echo " -> emscripten build FAILED"
    cd ..

    if [ "$WEB_ONLY" = false ]; then
        # psp build
        cd build-psp
        echo " -> building psp..."
        {
            psp-cmake -DBUILD_TARGET=PSP -DCMAKE_BUILD_TYPE=Release -G Ninja ..
            ninja
        } > /dev/null 2>&1 || echo " -> psp build FAILED"
        cd ..
    fi

    cd "$SCRIPT_DIR"
    echo "--> Done building: $example"
done
echo "All examples built."