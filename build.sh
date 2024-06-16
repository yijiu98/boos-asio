##!/bin/bash
set -e
build()
{
    if [ ! -x "build" ]; then
        echo "create 'build' folder"
        mkdir -p build
    fi
    
    if [ ! -x "out" ]; then
        echo "create 'out' folder"
        mkdir -p out
    fi

    echo "enter 'out' folder"
    cd out

    echo "generate makefile with flags and target toolchain"
    # cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=/opt/arm_toolchain_allwinner_a40i/x64-arm-Linux.cmake
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++

    echo "build (ie 'make')"
    cmake --build .
}


builduttest()
{
    if [ ! -x "build" ]; then
        echo "create 'build' folder"
        mkdir -p build
    fi
    
    if [ ! -x "out" ]; then
        echo "create 'out' folder"
        mkdir -p out
    fi

    echo "enter 'out' folder"
    cd out

    echo "generate makefile with flags and target toolchain"
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=/opt/arm_toolchain_allwinner_a40i/x64-arm-Linux.cmake -DUT_TEST=ON

    echo "build (ie 'make')"
    cmake --build .
}


clean()
{
    if [ -x "build" ]; then
        echo "remove 'build' folder"
        rm -rf build
    fi
    
    if [ -x "out" ]; then
        echo "remove 'out' folder"
        rm -rf out
    fi
}


case "$1" in
    clean)
        echo "clean:"
        clean
        ;;
    ut)
        echo "ut:"
        builduttest
        ;;
    *)
        echo "build:"
        build
        ;;
esac
