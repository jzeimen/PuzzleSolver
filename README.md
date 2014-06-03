PuzzleSolver
============

This is a program that uses computer vision techniques to solve jigsaw puzzles using the shapes of the edges.


# Compiling opencv and run

I am specifically using the 2.4.4 version of the opencv library. Here is how I have done it on 2 different computers. This is required to run PuzzleSolver. 

## OSX

    cd ~
    git clone git://code.opencv.org/opencv.git
    cd opencv
    git checkout 2.4.5
    mkdir build
    cd build
    cmake -G "Unix Makefiles" -D CMAKE_OSX_ARCHITECTURES=x86_64 ..
    make -j8
    sudo make install

#### Mavericks?
See https://github.com/jzeimen/PuzzleSolver/issues/2

## Linux (Tested on Ubuntu 12.04)

    cd ~
    git clone git://code.opencv.org/opencv.git
    cd opencv
    git checkout 2.4.5
    mkdir build
    cd build
    cmake -G "Unix Makefiles" ..
    make -j8
    sudo make install


# Compiling PuzzleSolver

Once the opencv library has been installed, you can try to open the xcodeproject, or on linux or mac run this from the PuzzleSolver directory (with the source files):

    g++ -O3 `pkg-config --cflags opencv` -o PuzzleSolver *.cpp `pkg-config --libs opencv`

This will result in an executable called PuzzleSolver. Currently the input/output settings are just hard coded in the program. 
