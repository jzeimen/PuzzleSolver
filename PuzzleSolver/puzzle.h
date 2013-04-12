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
    struct match_score{
        uint16_t edge1, edge2;
        double cost;
        static bool compare(match_score a, match_score b){
            return a.cost<b.cost;
        }
    };

    std::vector<match_score> matches;
    std::vector<piece> extract_pieces(std::string path);
    std::vector<piece>  pieces;
    
    void fill_costs();
    
    std::string edgeType_to_s(edgeType e);
public:
    puzzle(std::string);
    void solve();
};



#endif /* defined(__PuzzleSolver__puzzle__) */
