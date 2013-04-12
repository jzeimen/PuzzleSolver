//
//  piece.h
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/5/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#ifndef __PuzzleSolver__piece__
#define __PuzzleSolver__piece__
#include <iostream>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "edge.h"
typedef std::vector<cv::Mat> imlist;



enum pieceType {CORNER, FRAME, MIDDLE};

class piece{
private:
    std::vector<cv::Point2f> corners;
    cv::Mat bw;
    pieceType type;
    void process();
    void find_corners();
    void extract_edges();
    void classify();
public:
    cv::Mat full_color;

    piece(cv::Mat color,cv::Mat bw);
    edge edges[4];
    pieceType get_type();

    
};



#endif /* defined(__PuzzleSolver__piece__) */
