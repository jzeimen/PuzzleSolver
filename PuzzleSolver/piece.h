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
class piece{
private:
    cv::Mat full_color;
    cv::Mat bw;
    void process();
    void find_corners();
public:
    piece(cv::Mat color,cv::Mat bw);
    
};



#endif /* defined(__PuzzleSolver__piece__) */
