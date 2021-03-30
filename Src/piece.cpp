//
//  piece.cpp
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/5/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#include "piece.h"
#include "adjust_corners.h"

#include <cassert>
#include <algorithm>
#include <fstream>

#include "compat_opencv.h"
#include "edge.h"
#include "utils.h"
#include "logger.h"

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

//This returns iterators from the first vector where the value is equal places in the second vector.
std::vector<std::vector<cv::Point>::iterator> find_all_in(std::vector<cv::Point>::iterator begin, std::vector<cv::Point>::iterator end, const std::vector<cv::Point2f> &v){
    
    std::vector<std::vector<cv::Point>::iterator> places;
    for(; begin != end; begin++){
        for(std::vector<cv::Point2f>::const_iterator i = v.begin(); i!=v.end(); i++){
            if(begin->x == i->x && begin->y == i->y) places.push_back(begin);
        }
    }
    return places;
}



piece::piece(uint piece_number, std::string id, cv::Mat color, cv::Mat black_and_white, params& _user_params) : user_params(_user_params) {
    this->piece_number = piece_number;
    this->id = id;
    this->full_color = color;
    this->bw = black_and_white;
}

uint piece::get_number() {
    return piece_number;
}

std::string piece::get_id() {
    return id;
}

void piece::process(){
    find_corners();
    extract_edges();
    classify();
}


// compute the total distance traveling from 0 to index1, index2, index3
template <class T>
double ts_distance(std::vector<cv::Point_<T>> corners, uint index1, uint index2, uint index3) {
    
    double result = utils::distance<T>(corners, 0, index1);
    result += utils::distance<T>(corners, index1, index2);
    result += utils::distance<T>(corners, index2, index3);
    result += utils::distance<T>(corners, index3, 0);
    return result;
}

// Produce a metric that determines the quality of the piece corners. This function 
// assumes 4 corners and returns 0 for a perfect rectangle, and higher values for 
// shapes that are less like a rectangle.
template <class T>
double compute_corners_quality(std::vector<cv::Point_<T>> corners) {

    if (corners.size() != 4) {
        return 2000.0 * std::fabs(corners.size() - 4);
    }
    
    // order the corners using a simplified shortest path algorithm.  We just
    // need the points in clockwise or counter-clockwise order starting anywhere.
    std::vector<cv::Point_<T>> cpoints; // corners ordered by shortest path
    cpoints.push_back(corners[0]);
    
    double tsd0 = ts_distance( corners, 1, 2, 3);
    double tsd1 = ts_distance( corners, 2, 1, 3);
    double tsd2 = ts_distance( corners, 1, 3, 2);
    
    if (tsd0 < tsd1 && tsd0 < tsd2) {
        cpoints.push_back(corners[1]);
        cpoints.push_back(corners[2]);
        cpoints.push_back(corners[3]);        
    } else if (tsd1 < tsd2) {
        cpoints.push_back(corners[2]);
        cpoints.push_back(corners[1]);
        cpoints.push_back(corners[3]);         
    } else {
        cpoints.push_back(corners[1]);
        cpoints.push_back(corners[3]);
        cpoints.push_back(corners[2]);         
    }
    
    // quality will be determined by comparing the interior angles to 90 degrees
    // and by comparing the lengths of opposite sides.
    double quality = 0.0;

    double side_length[4] = {0, 0, 0, 0};
    
    for (uint i = 0; i < 4; i++) {
        double angle_diff = utils::compute_angle<T>(cpoints, i) - 90.0;
        double corner_quality = angle_diff * angle_diff;
        quality += corner_quality;
        
        side_length[i] = utils::distance<T>(cpoints, i, i+1);
    }
    
    // sldiff is the percent difference between opposite side lengths.
    double sldiff = 100.0 * (side_length[0] - side_length[2]) / std::min<double>(side_length[0], side_length[2]);
    quality += (sldiff * sldiff);
    sldiff = sldiff = (side_length[1] - side_length[3]) / std::min<double>(side_length[1], side_length[3]);
    quality += (sldiff * sldiff);

    return quality;
}

std::string piece::corners_points_filename() {
    std::stringstream filename;
    filename << user_params.getOutputDir() << "corners-" << id << ".dat";
    return filename.str();
}

void piece::save_corners_points() {
    std::string filename = corners_points_filename();
    std::ofstream file;
    file.open(filename);
    if (file.fail()) {
        logger::stream() << "Failed to write " << filename << std::endl; logger::flush();
        return;
    }
        
    for (std::vector<cv::Point2f>::iterator it = corners.begin(); it != corners.end(); it++) {
        file << it->x << " " << it->y << std::endl;
    }
    file.close();    
}

bool piece::load_corners_points() {
    std::string filename = corners_points_filename();
    std::ifstream file;
    file.open(filename);
    if (!file.fail()) {
        corners.clear();
        for (int i = 0; i < 4 && !file.fail(); i++) {
            float x;
            float y;
            file >> x;
            file >> y;
            corners.push_back(cv::Point2f(x,y));
        }
        bool failed = file.fail();
        file.close();

        if (failed) {
            logger::stream() << "Failed to read " << filename << std::endl; logger::flush();
            corners.clear();
        } 
        else {
            return true;
        }
    }
    return false;
}

void piece::save_corners_image() {
    cv::Mat corners_img = full_color.clone();
    for(uint i = 0; i < corners.size(); i++ ) {
        circle( corners_img, corners[i], corners_img.size().width / 50, cv::Scalar(0,0,255), 2, 8, 0 );
    }
    std::stringstream out_file_name;
    out_file_name << user_params.getOutputDir() << "corners-" << id << ".png";
    cv::imwrite(out_file_name.str(), corners_img);    
}

//Gets the piece ready to use.
//This code has been adapted from http://docs.opencv.org/doc/tutorials/features2d/trackingmotion/corner_subpixeles/corner_subpixeles.html
void piece::find_corners(){
    
    if (load_corners_points()) {
        if (user_params.isSavingCorners()) {
            save_corners_image();
        }
        return;
    }
    
    //How close can 2 corners be?
    double minDistance = user_params.getEstimatedPieceSize();
    //How big of an area to look for the corner in.
    int blockSize = user_params.getFindCornersBlockSize();
    bool useHarrisDetector = true;
    double k = 0.04;

    double min =0;
    double max =1;
    int max_iterations = 100;
    bool found_all_corners = false;

    //Binary search, altering quality until exactly 4 corners are found.
    //Usually done in 1 or 2 iterations
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

    //Find the sub-pixel locations of the corners.
    cv::Size winSize = cv::Size( blockSize, blockSize );
    cv::Size zeroZone = cv::Size( -1, -1 );
    cv::TermCriteria criteria = cv::TermCriteria( COMPAT_CV_TERM_CRITERIA_EPS + COMPAT_CV_TERM_CRITERIA_MAX_ITER, 40, 0.001 );

    /// Calculate the refined corner locations
    cv::cornerSubPix( bw, corners, winSize, zeroZone, criteria );
    
    double cornersQuality = compute_corners_quality<float>(corners);
    if (cornersQuality > user_params.getMinCornersQuality()) {
        logger::stream() << "Warning: poor corners for piece " << id << ", quality: " << cornersQuality << std::endl; logger::flush();
        
        if (user_params.isAdjustingCorners()) {
            std::vector<cv::Point2f> edited_corners;
            if (adjust_corners(id, full_color, user_params.getGuiScale(), corners, edited_corners, user_params.isVerbose())) {
                corners = edited_corners;
                cornersQuality = compute_corners_quality<float>(corners);
                logger::stream() << "New corner quality for piece " << id << ", quality: " << cornersQuality << std::endl; logger::flush();
                save_corners_points();
            }
        }
    }
    
    // More debug stuff, this will mark the corners with a red circle and save the image
    if (user_params.isSavingCorners() /* || user_params.getMinCornersQuality() < cornersQuality */) {
        save_corners_image();
    }
    

    if (corners.size() < 4) {
        logger::stream() << "Only found " << corners.size() << " corners for piece " << id << std::endl; logger::flush();
        exit(2);
    }
}


void piece::extract_edges(){
    //Extract the contour,
    //TODO: probably should have this passed in from the puzzle, since it already does this
    //It was done this way b/c the contours don't correspond to the correct pixel locations
    //in this cropped version of the image.
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(bw.clone(), contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
    assert(corners.size() == 4);
    if( 1 != contours.size() ){
        logger::stream() << "Found incorrect number of contours (" << contours.size() << ") for piece" << id << std::endl; logger::flush();
        exit(3);
    }
    std::vector<cv::Point> contour = contours[0];

    contour = utils::remove_duplicates(contour);

    //out of all of the found corners, find the closest points in the contour,
    //these will become the endpoints of the edges
    for(uint i = 0; i<corners.size(); i++){
        double best = 10000000000;
        cv::Point2f closest_point = contour[0];
        for(uint j = 0; j<contour.size(); j++){
            double d = utils::distance<int>(corners[i],contour[j]);
            if(d<best){
                best = d;
                closest_point = contour[j];
            }
        }
        corners[i] = closest_point;
    }



    //We need the begining of the vector to correspond to the begining of an edge.
    std::rotate(contour.begin(),find_first_in(contour.begin(), contour.end(), corners),contour.end());
    
    assert(corners[0]!=corners[1] && corners[0]!=corners[2] && corners[0]!=corners[3] && corners[1]!=corners[2] &&
           corners[1]!=corners[3] && corners[2]!=corners[3]);


    
    std::vector<std::vector<cv::Point>::iterator> sections;
    sections = find_all_in(contour.begin(), contour.end(), corners);

    //Make corners go in the correct order
    for(int i = 0; i<4; i++){
        corners[i]=*sections[i];
    }

    
    assert(corners[1]!=corners[0] && corners[0]!=corners[2] && corners[0]!=corners[3] && corners[1]!=corners[2] &&
           corners[1]!=corners[3] && corners[2]!=corners[3]);
    
    edges[0] = edge(std::vector<cv::Point>(sections[0],sections[1]));
    edges[1] = edge(std::vector<cv::Point>(sections[1],sections[2]));
    edges[2] = edge(std::vector<cv::Point>(sections[2],sections[3]));
    edges[3] = edge(std::vector<cv::Point>(sections[3],contour.end()));

   
    
}







//Classify the type of piece
void piece::classify(){
    int count = 0;
    for(int i = 0; i<4; i++){
        if(edges[i].get_type() == OUTER_EDGE) count ++;
    }
    if(count ==0){
        type = MIDDLE;
    } else if (count == 1){
        type = FRAME;
    } else if (count == 2){
        type = CORNER;
    } else {
        logger::stream() << "Problem, found too many outer edges for piece" << id << std:: endl; logger::flush();
        exit(4);
    }
}

pieceType piece::get_type(){
    return type;
}

//Remember the paradigm is that we go in ccw order
//this rotates it ccw "90 degrees" for each "time"
void piece::rotate(int times){
    int times_to_rotate = times%4;
    std::rotate(edges, edges+times_to_rotate, edges+4);
    std::rotate(corners.begin(), corners.begin()+times_to_rotate, corners.end());
}

cv::Point2f piece::get_corner(int id){
    return corners[id];
}



