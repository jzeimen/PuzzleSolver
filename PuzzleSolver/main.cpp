//
//  main.cpp
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/4/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#include <iostream>
#include <string.h>
#include "puzzle.h"
#include <cassert>
#include "PuzzleDisjointSet.h"
#include <sys/time.h>

static const std::string input = "/Users/jzeimen/Documents/school/College/Spring2013/ComputerVision/FinalProject/Scans/Angry Birds/color/";
static const std::string output = "/tmp/final/finaloutput.png";




int main(int argc, const char * argv[])
{//2.731
    
    std::cout << "Starting..." << std::endl;
    
    //44 for back
    //22 for color
    puzzle puzzle(input, 300, 30);

    timeval time;
    gettimeofday(&time, NULL);
    long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);

    puzzle.solve();
    gettimeofday(&time, NULL);
    std::cout << std::endl << "time to solve:"  << (((time.tv_sec * 1000) + (time.tv_usec / 1000))-millis)/1000.0 << std::endl;
    puzzle.save_image(output);

    gettimeofday(&time, NULL);
    std::cout << std::endl << "total time:"  << (((time.tv_sec * 1000) + (time.tv_usec / 1000))-millis)/1000.0 << std::endl;

    system("/usr/bin/open /tmp/final/finaloutput.png");
    
    return 0;
}


