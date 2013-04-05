//
//  main.cpp
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/4/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#include <iostream>
#include <vector>
#include <string.h>
#include <sys/types.h>
#include <sstream>
#include <dirent.h>
#include <vector>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "piece.h"

typedef std::vector<cv::Mat> imlist;
cv::RNG rng(12345);
static const std::string folderpath = "/Users/jzeimen/Documents/school/College/Spring2013/ComputerVision/FinalProject/Scans/Angry Birds/Scanner Open/";

//This function takes a directory, and returns a vector of every image opencv could extract from it.
imlist getImages(std::string path){
    imlist v;
    
    DIR *dp;
    struct dirent *ep;
    dp = opendir (path.c_str());
    
    if (dp != NULL)
    {
        while ((ep = readdir(dp))){
            cv::Mat image = cv::imread(path+ep->d_name);
            if(image.data!=NULL) v.push_back(image);
        }
        closedir(dp);
    }
    else{
        std::cout << "Couldn't open the directory" << std::endl;
        
        exit(1);
    }
    return v;
}


//Easy way to take a list of images and create a bw image at a specified threshold.
imlist color_to_bw(imlist color, int threshold){
    imlist black_and_white;
    for(imlist::iterator i = color.begin(); i != color.end(); i++){
        cv::Mat bw;
        cv::cvtColor(*i, bw, CV_BGR2GRAY);
        cv::threshold(bw, bw, threshold, 255, cv::THRESH_BINARY);
        black_and_white.push_back(bw);
    }
    return black_and_white;
}

//Performs a open then a close operation in order to remove small anomolies.
void filter(imlist to_filter, int size){
    cv::Mat k = cv::getStructuringElement(cv::MORPH_ELLIPSE,cv::Size(size,size));
    for(imlist::iterator i = to_filter.begin(); i != to_filter.end(); i++){
        cv::Mat bw;
        cv::morphologyEx(*i, bw, CV_MOP_OPEN, k);
        cv::morphologyEx(bw, *i, CV_MOP_CLOSE, k);
    }
}

std::vector<piece> extract_pieces(std::string path){
    std::vector<piece> pieces;
    imlist color_images = getImages(path);
    std::cout << "Found "<< color_images.size() << " Images." << std::endl;
    
    imlist bw = color_to_bw(color_images,45);
    std::cout << "Converted " << bw.size() << " to black and white" << std::endl;
    
    //Filter the noise out of the image
    filter(bw,4);

    //For each input image
    for(int i = 0; i<color_images.size(); i++){
        std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(bw[i].clone(), contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
        std::cout << "Found " << contours.size() <<  " contour(s)." << std::endl;
        
        //For each contour in that image
        for(int j = 0; j<contours.size(); j++){
            int bordersize = 10;
            cv::Rect r =  cv::boundingRect(contours[j]);
            r.width += bordersize*2;
            r.height += bordersize*2;
            r.x -= bordersize;
            r.y -= bordersize;
            
            
            cv::Mat mini_color = color_images[i](r);
            cv::Mat mini_bw = bw[i](r);
            //Create a copy so it can't conflict.
            mini_color = mini_color.clone();
            mini_bw = mini_bw.clone();
            
            piece p(mini_color, mini_bw);
            pieces.push_back(p);

        }
    }
    
    return pieces;
}

int main(int argc, const char * argv[])
{
    std::cout << "Starting..." << std::endl;

    
    extract_pieces(folderpath);

    // Set the neeed parameters to find the refined corners

    std::cout << "Finished\n";
    return 0;
}

