//
//  piece.cpp
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/5/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#include "piece.h"

int number = 0;

piece::piece(cv::Mat color, cv::Mat black_and_white){
    full_color = color;
    bw = black_and_white;
    process();
}



//Gets the piece ready to use.
//This code has been adapted from http://docs.opencv.org/doc/tutorials/features2d/trackingmotion/corner_subpixeles/corner_subpixeles.html
void piece::find_corners(){
    
    //Setup constants for
    std::vector<cv::Point2f> corners;
    //How close can 2 corners be?
    double minDistance = 200;
    //How big of an area to look for the corner in.
    int blockSize = 15;
    bool useHarrisDetector = false;
    double k = 0.04;
    
    double min =0;
    double max =1;
    int max_iterations = 10;
    bool found_all_corners = false;
    while(0<max_iterations--){
        corners.clear();
        double qualityLevel = (min+max)/2;
        std::cout << qualityLevel << std::endl;
        cv::goodFeaturesToTrack(bw.clone(),
                                corners,
                                100,
                                qualityLevel,
                                minDistance,
                                cv::Mat(),
                                blockSize,
                                useHarrisDetector,
                                k);
        
        
        if(corners.size() > 4){
            //Found too many corners increase quality
            min = qualityLevel;
            
        } else if (corners.size() < 4){
            max = qualityLevel;
        } else {
            //found all corners
            std::cout << "Found 4 corners" << std::endl;
            found_all_corners = true;
            break;
        }
        
        
    }
    
    
    if(found_all_corners){
        std::cout << "Success" << std::endl;
    } else {
        std::cerr << "Failed to find correct number of corners" << std::endl;
        exit(2);
    }
    
    
    //Find the sub-pixel locations of the corners.
    cv::Size winSize = cv::Size( 15, 15 );
    cv::Size zeroZone = cv::Size( -1, -1 );
    cv::TermCriteria criteria = cv::TermCriteria( CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 40, 0.001 );

    /// Calculate the refined corner locations
    cv::cornerSubPix( bw, corners, winSize, zeroZone, criteria );

    int r = 4;
    for( int i = 0; i < corners.size(); i++ )
    { circle( full_color, corners[i], r, cv::Scalar(255,255,255), -1, 8, 0 ); }

    std::stringstream out_file_name;
    out_file_name << "/tmp/final/test"<<number++<<".png";
    cv::imwrite(out_file_name.str(), full_color);

    
    
    
    
}

void piece::process(){
    find_corners();
}



