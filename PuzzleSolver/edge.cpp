//
//  edge.cpp
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/5/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#include "edge.h"
#include <vector>

edge::edge(std::vector<cv::Point> edge){
    contour = edge;
    std::cout << "Edge created with " << contour.size() << " points." << std::endl;
    //Todo: Need to standardize edges...
}

edge::edge(){
}