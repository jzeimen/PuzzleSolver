//
//  puzzle.cpp
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/5/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#include "puzzle.h"
#include <opencv/cv.h>

/*
                   _________      ____
                   \        \    /    |
                    |       /    \   /   _
                 ___/       \____/   |__/ \
                /       PUZZLE SOLVER      }
                \__/\  JOE ___ ZEIMEN  ___/
                     \    /   /       /
                     |    |  |       |
                    /_____/   \_______\


*/



/*
Needed:
 list of pieces with index number n
 list of edges with index number m and piece number = m/4, edge on piece = m%4
 
 Struct to hold costs, edge indexes (float,uint16,uint16)
 vector of said structs to be sorted and iterated through
 
 
 disjoint set, representitive of which holds the information about all other pieces...
 id_number=n, 
 rep_id_number=-1 if rep
 cv::Mat pieces
 cv::Mat rotations

*/

class blob{
    blob *parent;
    cv::Mat pieces;
    cv::Mat rotations;
};