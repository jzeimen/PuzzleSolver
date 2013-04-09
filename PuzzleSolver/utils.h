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
#endif /* defined(__PuzzleSolver__Utils__) */
