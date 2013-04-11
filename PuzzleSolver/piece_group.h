//
//  clump.h
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/10/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#ifndef __PuzzleSolver__piece_group__
#define __PuzzleSolver__piece_group__

#include <iostream>
#include <opencv/cv.h>
#include "puzzle.h"
#include "opencv/highgui.h"

class piece_group{
private:
    short id;
    
    //This datastructure acts like a dis-joint set, this is the id of its representitive.
    short representitave;
    
    //This class is really just a conveninet way to lump related info for the solving of a puzzle.
    friend class puzzle;
    
    //This matrix keeps the relative locations of the pieces as
    //a grid of the id numbers to said pieces
    cv::Mat_<int> locations;
    
    //The rotations keeps track of the rotations for each of the pieces.
    cv::Mat_<int> rotations;
    
public:
    piece_group(short id);
    static void test();
    //Returns true if the merge was successful
    bool merge(piece_group, int how);
    void rotate_ccw(int times);
    short get_representitave(){return representitave;};
    
};


#endif /* defined(__PuzzleSolver__piece_group__) */
