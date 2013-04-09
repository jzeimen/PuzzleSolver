//
//  puzzle.h
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/5/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#ifndef __PuzzleSolver__puzzle__
#define __PuzzleSolver__puzzle__

#include <iostream>
#include "edge.h"
#include "piece.h"
#include "utils.h"
class puzzle{
private:
    std::string edgeType_to_s(edgeType e);
    std::vector<piece> extract_pieces(std::string path);
    std::vector<piece>  pieces;
    
public:
    puzzle(std::string);
    void run();
};



#endif /* defined(__PuzzleSolver__puzzle__) */
