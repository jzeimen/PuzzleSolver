//
//  clump.cpp
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/10/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#include "piece_group.h"
#include <opencv/cv.h>

piece_group::piece_group(short new_id){
    id = new_id;
    representitave = -1;
    
    locations = cv::Mat_<int>(1,1,new_id);
    rotations = cv::Mat_<int>(1,1,0);

}




void piece_group::rotate_ccw(int times){
    int direction = times%4;
    switch (direction) {
        case 0: //Don't rotate
            return;
            break;
        case 1: //Rotate ccw 90 degrees
            cv::flip(locations,locations,1); //flip around y axis
            cv::transpose(locations,locations);
            cv::flip(rotations,rotations,1);
            cv::transpose(rotations, rotations);
            rotations+=1;

            break;
        case 2: //rotate 180
            cv::flip(locations,locations,-1); //flip around both axises
            cv::flip(rotations ,rotations,-1); //flip around both axises
            //basically rotations = (rotations+1)%4
            rotations+=2;
            break;
        case 3: //rotate cw 90 degrees
            cv::transpose(locations,locations);
            cv::flip(locations,locations,1); //flip around y axis
            cv::transpose(rotations, rotations);
            cv::flip(rotations,rotations,1);
            rotations+=3;
            break;
        default://Should never get here!!!
            return;
            break;
    }
    //basically moding by 4
    cv::bitwise_and(rotations, 0x3, rotations);
    return;
}


void piece_group::test(){
    piece_group p(1);
    p.locations = cv::Mat_<int>(2,3);
    p.rotations = cv::Mat_<int>(2,3);
    p.locations(0,0) = 1; p.locations(0,1) = 2; p.locations(0,2) = 3;
    p.locations(1,0) = 4; p.locations(1,1) = 5; p.locations(1,2) = 6;
    p.rotations(0,0) = 1; p.rotations(0,1) = 2; p.rotations(0,2) = 3;
    p.rotations(1,0) = 4; p.rotations(1,1) = 5; p.rotations(1,2) = 6;
    
    std::cout<< p.locations <<std::endl;
    p.rotate_ccw(0);
    std::cout<< p.locations;
    
}