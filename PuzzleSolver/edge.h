//
//  edge.h
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/5/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#ifndef __PuzzleSolver__edge__
#define __PuzzleSolver__edge__

#include <iostream>
#include <opencv/cv.h>


enum edgeType { OUTER_EDGE, TAB, HOLE };
class edge{
private:
    //The original contour passed into the function.
    std::vector<cv::Point> contour;
    //Normalized contour produces a contour that has its begining at (0,0)
    //and its endpoint straight above it (0,y). This is used internally
    //to classify the piece.
    std::vector<cv::Point2f> normalized_contour;
    void normalize();
    void classify();
    edgeType type;
public:
    edge();
    edge(std::vector<cv::Point> edge);
    std::vector<cv::Point> get_translated_contour(int,int);
    edgeType get_type();
    
};





#endif /* defined(__PuzzleSolver__edge__) */
