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
#include <dirent.h>
#include <vector>
#include "opencv/cv.h"
#include "opencv/highgui.h"

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



int main(int argc, const char * argv[])
{
    std::cout << "Starting..." << std::endl;
    cv::Mat m = cv::imread(folderpath);
    imlist color_images = getImages(folderpath);
    std::cout << "Found "<< color_images.size() << " Images." << std::endl;
    imlist bw = color_to_bw(color_images,45);
    std::cout << "Converted " << bw.size() << " to black and white" << std::endl;

    filter(bw,4);

    cv::imwrite("/Users/jzeimen/Desktop/test1.png", bw[1]);

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(bw[0].clone(), contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
    std::cout << "Found " << contours.size() <<  " contour(s)." << std::endl;
    
    
    std::vector<cv::Point2f> corners;
    double qualityLevel = 0.385;
    double minDistance = 100;
    int blockSize = 15;
    bool useHarrisDetector = false;
    double k = 0.04;
    
    cv::goodFeaturesToTrack(bw[0].clone(),
                            corners,
                            100,
                            qualityLevel,
                            minDistance,
                            cv::Mat(),
                            blockSize,
                            useHarrisDetector,
                            k);
    std::cout << "Found " << corners.size() << " corner(s)." << std::endl;
    
    

    /// Set the neeed parameters to find the refined corners
    cv::Size winSize = cv::Size( 15, 15 );
    cv::Size zeroZone = cv::Size( -1, -1 );
    cv::TermCriteria criteria = cv::TermCriteria( CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 40, 0.001 );
    
    /// Calculate the refined corner locations
    cv::cornerSubPix( bw[0], corners, winSize, zeroZone, criteria );
    
    int r = 4;
    for( int i = 0; i < corners.size(); i++ )
    { circle( color_images[0], corners[i], r, cv::Scalar(255,255,255), -1, 8, 0 ); }
    
    
    
    cv::imwrite("/Users/jzeimen/Desktop/test1.png", color_images[0]);
    std::cout << "Finished\n";
    return 0;
}

