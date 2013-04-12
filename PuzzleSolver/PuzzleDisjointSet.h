//
//  PuzzleDisjointSet.h
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/12/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#ifndef __PuzzleSolver__PuzzleDisjointSet__
#define __PuzzleSolver__PuzzleDisjointSet__

#include <iostream>
#include <opencv/cv.h>

class PuzzleDisjointSet{
public:
    struct forest{
        cv::Mat_<int> locations;
        cv::Mat_<int> rotations;
        int representative;
        int id;
    };
private:
    //A count of how many sets are left.
    int set_count;
    std::vector<forest> sets;
    void rotate_ccw(int id, int times);
    void make_set(int x);
    cv::Point find_location(cv::Mat_<int>, int number );
public:
    PuzzleDisjointSet( int number);
    bool join_sets(int a, int b, int how_a, int how_b);
    int find(int a);
    bool in_same_set(int a, int b);
    bool in_one_set();
    forest get(int id);
    


};





#endif /* defined(__PuzzleSolver__PuzzleDisjointSet__) */
