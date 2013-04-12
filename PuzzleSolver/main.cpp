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

static const std::string folderpath = "/Users/jzeimen/Documents/school/College/Spring2013/ComputerVision/FinalProject/Scans/Angry Birds/Scanner Open/";


void test(){
    PuzzleDisjointSet pds(17);
    
    assert(pds.find(1)==1);
    
    
    assert(pds.join_sets(9, 3, 0, 0));
    assert(pds.join_sets(9, 15, 2, 3));
    assert(pds.join_sets(3, 2, 3, 1));
    assert(pds.join_sets(8, 9, 3, 1));
    
    std::cout << pds.get(pds.find(3)).locations << std::endl;
    std::cout << pds.get(pds.find(3)).rotations << std::endl;
    
    
}


int main(int argc, const char * argv[])
{
    std::cout << "Starting..." << std::endl;
    timeval time;
    gettimeofday(&time, NULL);
    long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);

    puzzle puzzle(folderpath);
    puzzle.solve();
    gettimeofday(&time, NULL);
    
    std::cout << std::endl << "time:"  << (((time.tv_sec * 1000) + (time.tv_usec / 1000))-millis)/1000.0 << std::endl;
    
    

    return 0;
}


