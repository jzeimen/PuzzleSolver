/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   image_viewer.h
 * Author: kellinwood
 *
 * Created on April 4, 2019, 6:43 PM
 */

#ifndef IMAGE_VIEWER_H
#define IMAGE_VIEWER_H


#include <string>
#include "compat_opencv.h"

// Show the image in a GUI window
void show_image(std::string window_name, cv::Mat& image1);

// Show and allow the user to switch between two images.
void show_images(std::string window_name, cv::Mat& image1, cv::Mat& image2);      


#endif /* IMAGE_VIEWER_H */

