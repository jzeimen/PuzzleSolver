//
//  edge.cpp
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/5/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#include "edge.h"
#include <vector>
#include <cmath>
edge::edge(std::vector<cv::Point> edge){
    contour = edge;
    normalize();
    classify();
}


void edge::classify(){
    
    //See if it is an edge
    double contour_length = cv::arcLength(normalized_contour, false);
    double begin_end_distance = cv::norm(normalized_contour.front()-normalized_contour.back());
    if(contour_length < begin_end_distance*1.1){
        type = OUTER_EDGE;
        return;
    }
    
    
    int minx  = 10000000;
    int maxx = -100000000;
    for(int i = 0; i<normalized_contour.size(); i++){
        if(minx > normalized_contour[i].x)  minx = normalized_contour[i].x;
        if(maxx < normalized_contour[i].x)  maxx = normalized_contour[i].x;
    }
    
    if(abs(minx) > abs(maxx)){
        type = TAB;
    }else{
        type = HOLE;
    }
    
    
    
}

std::vector<cv::Point> edge::get_translated_contour(int offset_x, int offset_y){
    std::vector<cv::Point> ret_contour;
    cv::Point2f offset(offset_x,offset_y);
    for(int i = 0; i<normalized_contour.size(); i++){
        int x = (int)(normalized_contour[i].x+offset_x+0.5);
        int y = (int)(normalized_contour[i].y+offset_y+0.5);
        ret_contour.push_back(cv::Point(x,y));
    }
    return ret_contour;
};

void edge::normalize(){
    cv::Point2d a(contour.front().x,contour.front().y);
    cv::Point2d b(contour.back().x, contour.back().y);
    
    
    
    //Calculating angle from vertical
    b.x =b.x - a.x;
    b.y =b.y - a.y;
    double theta = std::acos(1.0*b.y/(cv::norm(b)));
    if(b.x < 0) theta = -theta;
    
    
    //Theta is the angle every point needs rotated.
    //and -a is the translation
    
    for(std::vector<cv::Point>::iterator i = contour.begin(); i!= contour.end(); i++ ){
        //Apply translation
        cv::Point2d temp_point(i->x-a.x,i->y-a.y);
        //Apply roatation
        double new_x= std::cos(theta) * temp_point.x - sin(theta)*temp_point.y;
        double new_y = std::sin(theta) * temp_point.x + std::cos(theta) *temp_point.y;
        normalized_contour.push_back(cv::Point2f((float)new_x, (float)new_y));
    }
    
    
    
    
    
}
edge::edge(){
}

edgeType edge::get_type(){
    return type;
}