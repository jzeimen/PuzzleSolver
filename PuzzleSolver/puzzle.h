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
#include <string>
#include "utils.h"

class puzzle{
private:
    struct match_score{
        uint16_t edge1, edge2;
        double score;
        static bool compare(match_score a, match_score b){
            return a.score<b.score;
        }
    };
    int threshold;
    bool solved;
    std::vector<match_score> matches;
    std::vector<piece> extract_pieces(std::string path, bool needs_filter);
    std::vector<piece>  pieces;
    cv::Mat_<int> solution;
    void print_edges();
    cv::Mat_<int> solution_rotations;
    void fill_costs();
    int piece_size;
    std::string edgeType_to_s(edgeType e);
public:
    puzzle(std::string path,int estimated_piece_size, int threshold, bool filter = true);
    void solve();
    void save_image(std::string filepath);
};



#endif /* defined(__PuzzleSolver__puzzle__) */
