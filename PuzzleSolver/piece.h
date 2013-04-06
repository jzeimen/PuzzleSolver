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
class piece{
private:
    cv::Mat full_color;
    std::vector<cv::Point2f> corners;
    cv::Mat bw;
    
    void process();
    void find_corners();
    void extract_edges();
public:
    piece(cv::Mat color,cv::Mat bw);
    edge edges[4];

    
};



#endif /* defined(__PuzzleSolver__piece__) */
