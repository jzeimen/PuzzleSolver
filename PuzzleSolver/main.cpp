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

#include <sys/time.h>

static const std::string folderpath = "/Users/jzeimen/Documents/school/College/Spring2013/ComputerVision/FinalProject/Scans/Angry Birds/Scanner Open/";

int main(int argc, const char * argv[])
{
    timeval time;
    gettimeofday(&time, NULL);
    long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);

//    puzzle puzzle(folderpath);
//    puzzle.solve();
    gettimeofday(&time, NULL);
    
    std::cout << std::endl << "time:"  << (((time.tv_sec * 1000) + (time.tv_usec / 1000))-millis)/1000.0 << std::endl;
    
    

    piece_group::test();


    
    return 0;
}


