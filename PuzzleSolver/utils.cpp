//
//  Utils.cpp
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/9/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#include "utils.h"




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
//            std::cout << path+ep->d_name << std::endl;
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
        //Opening and closing removes anything smaller than size
        cv::morphologyEx(*i, bw, CV_MOP_OPEN, k);
        cv::morphologyEx(bw, *i, CV_MOP_CLOSE, k);
    }
}

//Performs a open then a close operation in order to remove small anomolies.
imlist blur(imlist to_filter, int size, double sigma){
    imlist ret;
    for(imlist::iterator i = to_filter.begin(); i != to_filter.end(); i++){
        cv::Mat m;
        cv::GaussianBlur(*i, m, cv::Size(size,size), sigma);
        ret.push_back(m);
    }
    return ret;
}


//Performs a open then a close operation in order to remove small anomolies.
imlist median_blur(imlist to_filter, int k){
    imlist ret;
    for(imlist::iterator i = to_filter.begin(); i != to_filter.end(); i++){
        cv::Mat m;
        cv::medianBlur(*i, m, k);
        ret.push_back(m);
    }
    return ret;
}

imlist bilateral_blur(imlist to_blur){
    imlist ret;
    for(imlist::iterator i = to_blur.begin(); i != to_blur.end(); i++){
        cv::Mat m;
        cv::bilateralFilter(*i, m, 5, 152, 5);

        cv::imwrite("/tmp/final/bilat.png", m);
        cv::imwrite("/tmp/final/before_bilat.png", *i);
        ret.push_back(m);
    }
    return ret;
}
std::vector<cv::Point> remove_duplicates(std::vector<cv::Point> vec){
    bool dupes_found = true;
    while(dupes_found){
        dupes_found=false;
        int dup_at=-1;
        for(int i =0; i<vec.size(); i++){
            for(int j=0; j<vec.size(); j++){
                if(j==i) continue;
                //                std::cout << vec[i] << " " << vec[j] << std::endl;
                if(vec[i] == vec[j]){
                    dup_at=j;
                    dupes_found = true;
                    vec.erase(vec.begin()+j);
                    break;
                }
            }
            if(dupes_found){
                break;
            }
        }
    }
    return vec;
}


