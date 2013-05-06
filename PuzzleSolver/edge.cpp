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
#include "utils.h"
edge::edge(std::vector<cv::Point> edge){
    //original
    contour = edge;
    
    //Normalized contours are used for comparisons
    normalized_contour = normalize(contour);
    std::vector<cv::Point> copy(contour.begin(),contour.end());
    std::reverse(copy.begin(), copy.end());
    //same as normalized contour, but flipped 180 degrees
    reverse_normalized_contour = normalize(copy);
    classify();
}


//Trying OpenCV's match shapes, hasn't worked as well as my compare2 function.
double edge::compare(edge that){
    //Return large numbers if we know that these shapes simply wont match...
    if(type == OUTER_EDGE || that.type == OUTER_EDGE) return 1000000;
    if(type == that.type) return 10000000;
    return cv::matchShapes(contour, that.contour, CV_CONTOURS_MATCH_I2, 0);
}


//This comparison iterates over every point in "this" contour,
//finds the closest point in "that" contour and sums those distances up.
//The end result is the sum divided by length of the 2 contours
double edge::compare2(edge that){
    //Return large number if an impossible situation is happening
    if(type == OUTER_EDGE || that.type == OUTER_EDGE) return 100000000;
    if(type == that.type) return 100000000;
    double cost=0;
    double total_length =  cv::arcLength(normalized_contour, false) + cv::arcLength(that.reverse_normalized_contour, false);
    
    for(std::vector<cv::Point2f>::iterator i = normalized_contour.begin(); i!=normalized_contour.end(); i++){
        double min = 10000000;
        for(std::vector<cv::Point2f>::iterator j = that.reverse_normalized_contour.begin(); j!=that.reverse_normalized_contour.end(); j++){
            double dist = std::sqrt(std::pow(i->x - j->x,2) + std::pow(i->y - j->y, 2));
            if(dist<min) min = dist;
        }
        
        cost+=min;
    }
    return cost/total_length;
}



void edge::classify(){
    
    //See if it is an outer edge comparing the distance between beginning and end w/
    //the arc length.
    double contour_length = cv::arcLength(normalized_contour, false);
    double begin_end_distance = cv::norm(normalized_contour.front()-normalized_contour.back());
    if(contour_length < begin_end_distance*1.3){
        type = OUTER_EDGE;
        return;
    }
    
    //Find the minimum or maximum value for x in the normalized contour and base
    //the classification on that
    int minx  = 100000000;
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
    return translate_contour(normalized_contour, offset_x, offset_y);
};

std::vector<cv::Point> edge::get_translated_contour_reverse(int offset_x, int offset_y){
    return translate_contour(reverse_normalized_contour, offset_x, offset_y);
};


//This function takes in a vector of points, and transforms it so that it starts at the origin,
//and ends on the y-axis
template<class T>
std::vector<cv::Point2f> edge::normalize(std::vector<T> cont){
    std::vector<cv::Point2f> ret_contour;
    cv::Point2d a(cont.front().x,cont.front().y);
    cv::Point2d b(cont.back().x, cont.back().y);
    
    //Calculating angle from vertical
    b.x =b.x - a.x;
    b.y =b.y - a.y;
    
    double theta = std::acos(b.y/(cv::norm(b)));
    if(b.x < 0) theta = -theta;
    
    //Theta is the angle every point needs rotated.
    //and -a is the translation
    for(std::vector<cv::Point>::iterator i = cont.begin(); i!= cont.end(); i++ ){
        //Apply translation
        cv::Point2d temp_point(i->x-a.x,i->y-a.y);
        //Apply roatation
        double new_x= std::cos(theta) * temp_point.x - sin(theta)*temp_point.y;
        double new_y = std::sin(theta) * temp_point.x + std::cos(theta) *temp_point.y;
        ret_contour.push_back(cv::Point2f((float)new_x, (float)new_y));
    }
    
    return ret_contour;
}

//TODO: get rid of this default constructor so it isn't accedentally used.
edge::edge(){
}

edgeType edge::get_type(){
    return type;
}


std::string edge::edge_type_to_s(){
    switch(type){
        case OUTER_EDGE: return "Edge";
        case TAB: return "Tab";
        case HOLE: return "Hole";
    }
    return "";
}

