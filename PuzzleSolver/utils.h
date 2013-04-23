//
//  Utils.h
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/9/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#ifndef __PuzzleSolver__Utils__
#define __PuzzleSolver__Utils__

#include <iostream>

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


void filter(imlist to_filter, int size);
imlist color_to_bw(imlist color, int threshold);
void filter(imlist to_filter, int size);
imlist getImages(std::string path);
imlist blur(imlist to_blur, int size, double sigma);
imlist median_blur(imlist to_blur, int size);
imlist bilateral_blur(imlist to_blur);

template<class T> std::vector<cv::Point> translate_contour(std::vector<T> in , int offset_x, int offset_y);
std::vector<cv::Point> remove_duplicates(std::vector<cv::Point> vec);
//Return a contour that is translated
template<class T>
std::vector<cv::Point> translate_contour(std::vector<T> in , int offset_x, int offset_y){
    std::vector<cv::Point> ret_contour;
    cv::Point2f offset(offset_x,offset_y);
    for(int i = 0; i<in.size(); i++){
        int x = (int)(in[i].x+offset_x+0.5);
        int y = (int)(in[i].y+offset_y+0.5);
        ret_contour.push_back(T(x,y));
    }
    return ret_contour;
}

#endif /* defined(__PuzzleSolver__Utils__) */
