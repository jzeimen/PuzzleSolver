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

#include <vector>
#include "compat_opencv.h"
#include "edge.h"
#include "params.h"

enum pieceType {CORNER, FRAME, MIDDLE};


//The paradigm for the piece is that there are 4 edges
//the edge "numbers" go from 0-3 in counter clockwise order
//starting from the left.



class piece{
private:
    uint piece_number;
    std::string id;
    std::vector<cv::Point2f> corners;
    pieceType type;

    void find_corners();
    void extract_edges();
    void classify();
    params& user_params;
    std::string corners_points_filename();
    void save_corners_points();
    bool load_corners_points();
    void save_corners_image();
public:
    cv::Mat full_color;
    cv::Mat bw;
    edge edges[4];
    piece(uint piece_number, std::string id, cv::Mat color, cv::Mat bw, params& user_params);
    void process();    
    uint get_number();
    std::string get_id();
    pieceType get_type();
    cv::Point2f get_corner(int id);
    
    //This method "rotates the corners and edges so they are in a correct order.
    void rotate(int times);

    
};



#endif /* defined(__PuzzleSolver__piece__) */
