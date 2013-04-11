//
//  puzzle.cpp
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/5/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#include "puzzle.h"
#include <opencv/cv.h>
#include <omp.h>


/*
                   _________      ____
                   \        \    /    |
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


//Function just for me to test and play with how the interface is going...
void puzzle::solve(){
    std::cout << "Starting..." << std::endl;
    
    
    
    int middle = 0;
    int frame = 0;
    int corner = 0;
    
    for(int i=0; i<pieces.size(); i++){
        
        
        for(int j=0; j<4; j++){
            cv::Mat img= cv::Mat::zeros(500,500, CV_8UC1);
            
            std::vector<std::vector<cv::Point> > contours;
            std::vector<cv::Point> contour = pieces[i].edges[j].get_translated_contour(200,0);
            contours.push_back(contour);
            contour = pieces[11].edges[1].get_translated_contour_reverse(200,0);
            //            contours.push_back(contour);
            cv::drawContours(img, contours, -1, cv::Scalar(255), 1);
            
            cv::putText(img, pieces[i].edges[j].edgeType_to_s(), cv::Point(300,250), CV_FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255));
            
            std::stringstream out_name;
            out_name << "/tmp/final/contour-" << i << "-" << j << ".png";
            
            double compare_quality = pieces[11].edges[1].compare2(pieces[i].edges[j]);
            //            if(compare_quality < 50)
            std::cout << out_name.str() << " " << compare_quality << std::endl;
            cv::imwrite(out_name.str(),img);
        }
        switch(pieces[i].get_type()){
            case MIDDLE:
                middle++;
                break;
            case FRAME:
                frame++;
                break;
            case CORNER:
                corner++;
                break;
                
        }
    }
    
    
    std::cout << "Corners: " << corner << " frame: " << frame << " middle: " << middle << std::endl;
    
    
    double array[24*4][24*4];
    
    omp_set_num_threads(4);
#pragma omp parallel for
    for(int ip=0; ip<24; ip++){
        for(int ie=0; ie<4; ie++){
            for(int jp=0; jp<24; jp++){
                for(int je=0; je<4; je++){
                    array[ip*4+ie][jp*4+je]= pieces[ip].edges[ie].compare2(pieces[jp].edges[je]);
                }
            }
        }
        std::cout << ip << std::endl;
    }
    
    for(int i = 0; i<96; i++){
        double min_cost = 1023123;
        int min_pos = 0;
        for(int j = 0; j<96; j++){
            if(array[i][j] < min_cost){
                min_cost = array[i][j];
                min_pos = j;
            }
        }
        if(pieces[i/4].edges[i%4].get_type() == OUTER_EDGE) continue;
        cv::Mat img= cv::Mat::zeros(500,500, CV_8UC1);
        
        std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Point> contour = pieces[i/4].edges[i%4].get_translated_contour(200,0);
        contours.push_back(contour);
        contour = pieces[min_pos/4].edges[min_pos%4].get_translated_contour_reverse(200,0);
        contours.push_back(contour);
        cv::drawContours(img, contours, -1, cv::Scalar(255), 1);
        
        
        std::stringstream out_name;
        out_name << "/tmp/final/found_contour-" << i/4 << "-" << i%4 << ".png";
        
        
        cv::imwrite(out_name.str(),img);
        std::cout << (i/4) << "-" << i%4 << "   " << min_pos/4 << "-" << min_pos%4  << "\t" << array[i][min_pos]<< std::endl;
    }
    
    
    
    std::cout << "Finished\n";

}
/*
Still Needed:
 list of pieces with index number n
 list of edges with index number m and piece number = m/4, edge on piece = m%4
 
 Struct to hold costs, edge indexes (float,uint16,uint16)
 vector of said structs to be sorted and iterated through
 
 
 disjoint set, representitive of which holds the information about all other pieces...
 id_number=n, 
 rep_id_number=-1 if rep
 cv::Mat pieces
 cv::Mat rotations

*/
