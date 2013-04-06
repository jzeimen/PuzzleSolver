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
class edge{
    std::vector<cv::Point> contour;
public:
    edge();
    edge(std::vector<cv::Point> edge);
    
};

#endif /* defined(__PuzzleSolver__edge__) */
