//
//  piece.cpp
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/5/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#include "piece.h"
#include <cassert>
#include <algorithm>
#include "edge.h"

int number = 0;

//This function takes in the beginning and ending of one vector, and returns
//an iterator representing the point where the first item in the second vector is.
std::vector<cv::Point>::iterator find_first_in(std::vector<cv::Point>::iterator begin, std::vector<cv::Point>::iterator end, const std::vector<cv::Point2f> &v){
    for(; begin != end; begin++){
        for(std::vector<cv::Point2f>::const_iterator i = v.begin(); i!=v.end(); i++){
            if(begin->x == i->x && begin->y == i->y) return begin;
        }
    }
    return end;
}

//This returns iterators from the first vector where they equal places in the second vector. 
std::vector<std::vector<cv::Point>::iterator> find_all_in(std::vector<cv::Point>::iterator begin, std::vector<cv::Point>::iterator end, const std::vector<cv::Point2f> &v){
    
    std::vector<std::vector<cv::Point>::iterator> places;
    for(; begin != end; begin++){
        for(std::vector<cv::Point2f>::const_iterator i = v.begin(); i!=v.end(); i++){
            if(begin->x == i->x && begin->y == i->y) places.push_back(begin);
        }
    }
    return places;
}



double distance(cv::Point a, cv::Point b){
    return cv::norm(a-b);
}

piece::piece(cv::Mat color, cv::Mat black_and_white){
    full_color = color;
    bw = black_and_white;
    process();
}


void piece::process(){
    find_corners();
    extract_edges();
}



void piece::extract_edges(){
    
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(bw.clone(), contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
    assert(corners.size() == 4);
    if( 1 != contours.size() ){
        std::cerr << "Found incorrect number of contours." << std::endl;
        exit(3);
    }
    std::vector<cv::Point> contour = contours[0];

    
    for(int i = 0; i<corners.size(); i++){
        double best = 10000000000;
        cv::Point2f closest_point = contour[0];
        for(int j = 0; j<contour.size(); j++){
            double d = distance(corners[i],contour[j]);
            if(d<best){
                best = d;
                closest_point = contour[j];
            }
        }
        corners[i] = closest_point;
    }
    

    std::rotate(contour.begin(),find_first_in(contour.begin(), contour.end(), corners),contour.end());
    
    std::vector<std::vector<cv::Point>::iterator> sections;
    sections = find_all_in(contour.begin(), contour.end(), corners);

    //Make corners go in the correct order
    for(int i = 0; i<4; i++){
        corners[i]=*sections[i];
    }

    edges[0] = edge(std::vector<cv::Point>(sections[0],sections[1]));
    edges[1] = edge(std::vector<cv::Point>(sections[1],sections[2]));
    edges[2] = edge(std::vector<cv::Point>(sections[2],sections[3]));
    edges[3] = edge(std::vector<cv::Point>(sections[3],contour.end()));

   
    
}




//Gets the piece ready to use.
//This code has been adapted from http://docs.opencv.org/doc/tutorials/features2d/trackingmotion/corner_subpixeles/corner_subpixeles.html
void piece::find_corners(){
    

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
            found_all_corners = true;
            break;
        }

    }
    
    
    if(found_all_corners){
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





