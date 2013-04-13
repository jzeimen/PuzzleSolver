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
    pieceType type;
    void process();
    void find_corners();
    void extract_edges();
    void classify();
public:
    cv::Mat full_color;
    cv::Mat bw;

    piece(cv::Mat color,cv::Mat bw);
    edge edges[4];
    pieceType get_type();
    cv::Point2f get_corner(int id);
    
    //This method "rotates the corners and edges so they are in a correct order.
    void rotate(int times);

    
};



#endif /* defined(__PuzzleSolver__piece__) */
