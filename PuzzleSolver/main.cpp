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

//Dont forget final "/" in directory name.
static const std::string input = "/Users/jzeimen/Documents/school/College/Spring2013/ComputerVision/FinalProject/PuzzleSolver/PuzzleSolver/Scans/";
static const std::string output = "/tmp/final/finaloutput.png";


int main(int argc, const char * argv[])
{
    
    std::cout << "Starting..." << std::endl;
    
    //44 for back
    //22 for color
    
    //Toy Story Color & breaks with median filter, needs filter()
    puzzle puzzle(input+"Toy Story/", 200, 22, false);

    
    //Toy Story back works w/ median filter
//    puzzle puzzle(input+"Toy Story back/", 200, 50);
    
    //Angry Birds color works with median, or filter
//    puzzle puzzle(input+"Angry Birds/color/",300,30);

    //Angry Birds back works with median
//    puzzle puzzle(input+"Angry Birds/Scanner Open/",300,30);
    
      //Horses back not numbered
//    puzzle puzzle(input+"horses/", 380, 50);

    //Horses back numbered
//    puzzle puzzle(input+"horses numbered/", 380, 50);
    


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


