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


//The paradigm for edges is that if you walked along the edge of the contour
//from beginning to end, the piece will be to the left, and empty space to right.
class edge{
private:
    //The original contour passed into the function.
    std::vector<cv::Point> contour;
    //Normalized contour produces a contour that has its begining at (0,0)
    //and its endpoint straight above it (0,y). This is used internally
    //to classify the piece.
    std::vector<cv::Point2f> normalized_contour;
    std::vector<cv::Point2f> reverse_normalized_contour;
    template<class T> std::vector<cv::Point2f> normalize(std::vector<T>);
    void classify();
    edgeType type;
public:
    edge();
    edge(std::vector<cv::Point> edge);
    std::vector<cv::Point> get_translated_contour(int,int);
    std::vector<cv::Point> get_translated_contour_reverse(int,int);
    edgeType get_type();
    double compare(edge);
    double compare2(edge);
    std::string edge_type_to_s();
    
};





#endif /* defined(__PuzzleSolver__edge__) */
