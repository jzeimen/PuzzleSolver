//
//  puzzle.cpp
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/5/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#include "puzzle.h"
#include <opencv/cv.h>
#include "PuzzleDisjointSet.h"


/*
                   _________      _____
                   \        \    /    /
                    |       /    \   /   _
                 ___/       \____/   |__/ \
                /       PUZZLE SOLVER      }
                \__/\  JOE ___ ZEIMEN  ___/
                     \    /   /       /
                     |    |  |       |
                    /_____/   \_______\
*/


std::vector<piece> puzzle::extract_pieces(std::string path){
    std::vector<piece> pieces;
    imlist color_images = getImages(path);
    
    imlist bw = color_to_bw(color_images,45);
    std::cout << "Converted " << bw.size() << " to black and white" << std::endl;
    
    //Filter the noise out of the image
    filter(bw,4);
    
    //For each input image
    for(int i = 0; i<color_images.size(); i++){
        std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(bw[i].clone(), contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
        //        std::cout << "Found " << contours.size() <<  " contour(s)." << std::endl;
        
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

puzzle::puzzle(std::string folderpath){
    pieces = extract_pieces(folderpath);

}


void puzzle::fill_costs(){
    int no_edges = (int) pieces.size()*4;
    
    for(int i =0; i<no_edges; i++){
        for(int j=i; j<no_edges; j++){
            match_score score;
            score.edge1 =(int) i;
            score.edge2 =(int) j;
            score.cost = pieces[i/4].edges[i%4].compare2(pieces[j/4].edges[j%4]);
            matches.push_back(score);
        }
    }
    std::sort(matches.begin(),matches.end(),match_score::compare);
    std::cout << matches.begin()->cost;
}




//Function just for me to test and play with how the interface is going...
void puzzle::solve(){
    fill_costs();
    int output_id=0;
    std::vector<match_score>::iterator i= matches.begin();
    PuzzleDisjointSet p((int)pieces.size());
    
    
    for(int i=0; i<pieces.size(); i++){
        std::stringstream filename;
        filename << "/tmp/final/p" << i << ".png";
//        cv::imwrite(filename.str(), pieces[i].full_color);
    }
    
    while(!p.in_one_set() && i!=matches.end() ){
        int p1 = i->edge1/4;
        int e1 = i->edge1%4;
        int p2 = i->edge2/4;
        int e2 = i->edge2%4;
        
        
        cv::Mat m = cv::Mat::zeros(500,500,CV_8UC1);
        std::stringstream out_file_name;
        out_file_name << "/tmp/final/match" << output_id++ << "_" << p1<< "_" << e1 << "_" <<p2 << "_" <<e2 << ".png";
        std::vector<std::vector<cv::Point> > contours;
        contours.push_back(pieces[p1].edges[e1].get_translated_contour(200, 0));
        contours.push_back(pieces[p2].edges[e2].get_translated_contour_reverse(200, 0));
        cv::drawContours(m, contours, -1, cv::Scalar(255));
        std::cout << out_file_name.str() << std::endl;
//        cv::imwrite(out_file_name.str(), m);
        
        std::cout << "Attempting to merge: " << p1 << " with: " << p2 << " using edges:" << e1 << ", " << e2 << " c:" << i->cost <<  std::endl;
        p.join_sets(p1, p2, e1, e2);
        std::cout << p.get(p.find(p1)).locations << std:: endl;
        i++;
    }
    
    if(p.in_one_set()){
        std::cout << "Possible solution found" << std::
        endl;
    }
    
    
    
    
    //For the next best match, edge1 comes from piece A, edge2 comes from piece B
    //Find original matching. 1-4    left bottom right top
    //Pass in to merge
    //If merge was successfull count++
    //if count == number of pieces Done, return
    
    

}

