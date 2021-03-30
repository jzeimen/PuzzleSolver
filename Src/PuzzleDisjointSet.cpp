//
//  PuzzleDisjointSet.cpp
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/12/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#include "PuzzleDisjointSet.h"

#include <algorithm>
#include <ostream>
#include <iomanip>
#include <ctime>

#include "compat_opencv.h"
#include "logger.h"

PuzzleDisjointSet::PuzzleDisjointSet(params& user_params, int number, match_checker checker, void* match_check_data) 
  : user_params(user_params), edge_checker(checker), match_check_data(match_check_data) {
    set_count=0;
    merge_failures = 0;
    for(int i=0; i<number; i++){
        make_set(i);
    }

}


void PuzzleDisjointSet::make_set(int new_id){
    forest f;
    f.id = new_id;
    f.representative = -1;
    f.locations = cv::Mat_<int>(1,1,new_id);
    f.rotations = cv::Mat_<int>(1,1,0);
    sets.push_back(f);
    set_count++;
}

void PuzzleDisjointSet::init_join(PuzzleDisjointSet::join_context& c, int a, int b, int how_a, int how_b) {
    c.a = a;
    c.b = b;
    c.how_a = how_a;
    c.how_b = how_b;
    c.rep_a = find(a);
    c.rep_b = find(b);    
    c.joinable = c.rep_a != c.rep_b;
}


bool PuzzleDisjointSet::compute_join(PuzzleDisjointSet::join_context& c) {
    if (!c.joinable) return false; //Already in same set...
    
    c.joinable = false;

//    std::cout << std::endl << sets[rep_a].rotations << std::endl << sets[rep_b].rotations << std::endl;

    //We need A to have its adjoining edge to be to the right, position 2
    // meaning if its rotation was 0 it would need to be rotated by 2
    cv::Point loc_of_a = find_location(sets[c.rep_a].locations, c.a);
    int rot_a = sets[c.rep_a].rotations(loc_of_a);
    int to_rot_a = (6 - c.how_a - rot_a)%4;
    rotate_ccw(c.rep_a, to_rot_a);
    
    //We need B to have its adjoining edge to the left, position 0
    //if its position was 0, 
    cv::Point loc_of_b = find_location(sets[c.rep_b].locations, c.b);
    int rot_b = sets[c.rep_b].rotations(loc_of_b);
    int to_rot_b = (8-rot_b-c.how_b)%4;
    rotate_ccw(c.rep_b, to_rot_b);
    
    
    //figure out the size of the new Mats
    loc_of_a = find_location(sets[c.rep_a].locations, c.a);
    COMPAT_CV_MAT_SIZE size_of_a = sets[c.rep_a].locations.size;
    loc_of_b = find_location(sets[c.rep_b].locations, c.b);
    COMPAT_CV_MAT_SIZE size_of_b = sets[c.rep_b].locations.size;
    
    int width = std::max(size_of_a[1], loc_of_a.x - loc_of_b.x +1 +size_of_b[1]) - std::min(0, loc_of_a.x-loc_of_b.x +1);
    int height = std::max(size_of_a[0], loc_of_a.y - loc_of_b.y +size_of_b[0]) - std::min(0, loc_of_a.y-loc_of_b.y);
    
    
    //place old A and B into the new Mats of same size
    
    cv::Mat_<int> new_a_locs(height, width, -1);
    cv::Mat_<int> new_b_locs(height, width, -1);
    cv::Mat_<int> new_a_rots(height, width, 0);
    cv::Mat_<int> new_b_rots(height, width, 0);
    
    int ax_offset = std::abs(std::min(0, loc_of_a.x-loc_of_b.x+1));
    int ay_offset = std::abs(std::min(0, loc_of_a.y-loc_of_b.y));
    
    int bx_offset = -(loc_of_b.x -( loc_of_a.x+ax_offset+1));
    int by_offset = -(loc_of_b.y -(loc_of_a.y+ay_offset));
    
    sets[c.rep_a].locations.copyTo(new_a_locs(cv::Rect(ax_offset,ay_offset,size_of_a[1],size_of_a[0])));
    sets[c.rep_a].rotations.copyTo(new_a_rots(cv::Rect(ax_offset,ay_offset,size_of_a[1],size_of_a[0])));
    sets[c.rep_b].locations.copyTo(new_b_locs(cv::Rect(bx_offset,by_offset,size_of_b[1],size_of_b[0])));
    sets[c.rep_b].rotations.copyTo(new_b_rots(cv::Rect(bx_offset,by_offset,size_of_b[1],size_of_b[0])));


//    std::cout << "w: " << width << ", h: " << height << std::endl;
    
    //check for overlap while combining...
    for(int i = 0; i<new_a_locs.size[0]; i++){
        for(int j=0; j<new_a_locs.size[1]; j++){

            //If both have a real value for a piece, it becomes impossible, reject
            if(new_a_locs(i,j) != -1 && new_b_locs(i,j) != -1) {
                if (user_params.isVerbose()) {
                    logger::stream() << "Failed to merge because of overlap" << std::endl; logger::flush();
                    merge_failures++;
                }
                return false;
            }
            
            
            // Check adjoining edge matches and fail to merge if the match is low quality or impossible
            if (edge_checker != NULL && new_b_locs(i,j) != -1 && size_of_b[0] == 1 && size_of_b[1] == 1) {
                if (i > 0 && new_a_locs(i-1,j) != -1) {
//                    std::cout << "b("<<i<<","<<j<<")=" << (1+new_b_locs(i,j)) << ", " 
//                            << "a!("<<(i-1)<<","<<j<<")=" << (1+new_a_locs(i-1,j)) << ", " << std::endl;
                    if (!edge_checker(match_check_data, new_a_locs(i-1,j), new_b_locs(i,j), (5 - new_a_rots(i-1,j))%4, (7 - new_b_rots(i,j))%4)) {
                        match_failure();
                        return false;
                    }
                }
//                if (j > 0 && new_a_locs(i,j-1) != -1) {
//                    std::cout << "a("<<i<<","<<(j-1)<<")=" << (1+new_a_locs(i,j-1)) << ", ";
//                }               
                if (i < (new_a_locs.rows-1) && new_a_locs(i+1,j) != -1) {
//                    std::cout << "b("<<i<<","<<j<<")=" << (1+new_b_locs(i,j)) << ", " 
//                            << "a@("<<(i+1)<<","<<j<<")=" << (1+new_a_locs(i+1,j)) << ", " << std::endl;
                    if (!edge_checker(match_check_data, new_a_locs(i+1,j), new_b_locs(i,j), (7 - new_a_rots(i+1,j))%4, (5 - new_b_rots(i,j))%4)) {
                        match_failure();
                        return false;
                    }
                }
                if (j < (new_a_locs.cols-1) && new_a_locs(i,j+1) != -1) {
//                    std::cout << "b("<<i<<","<<j<<")=" << (1+new_b_locs(i,j)) << ", "
//                            << "a#("<<i<<","<<(j+1)<<")=" << (1+new_a_locs(i,j+1)) << ", " << std::endl;
                    if (!edge_checker(match_check_data, new_a_locs(i,j+1), new_b_locs(i,j), (4 - new_a_rots(i,j+1))%4, (6 - new_b_rots(i,j))%4)) {
                        match_failure();
                        return false;                        
                    }
                }                
//                std::cout << "b("<<i<<","<<j<<")=" << (1+new_b_locs(i,j)) << ", ";
            }

            if(new_a_locs(i,j) == -1 && new_b_locs(i,j) !=-1){
                new_a_locs(i,j) = new_b_locs(i,j);
                new_a_rots(i,j) = new_b_rots(i,j);
            }
            
        }
    }
    // std::cout << std::endl;

    c.new_a_locs = new_a_locs;
    c.new_a_rots = new_a_rots;
    c.joinable = true;
    return true;
}

void PuzzleDisjointSet::complete_join(join_context& c) {

    
    //Set the new representative a, to have this Mat
    sets[c.rep_a].locations = c.new_a_locs;
    sets[c.rep_a].rotations = c.new_a_rots;
    
    //Updating the number of sets left
    set_count--;
    
    std::vector<int>::iterator ci = std::find(csets.begin(), csets.end(), c.rep_a);
    if (ci == csets.end()) {
        csets.insert(csets.begin(), c.rep_a);
    }
    
    ci = std::find(csets.begin(), csets.end(), c.rep_b);
    if (ci != csets.end()) {
        csets.erase(ci);
    }
    
    //Representative is the same idea as a disjoint set datastructure
    sets[c.rep_b].representative = c.rep_a;
}

void PuzzleDisjointSet::match_failure() {
    if (user_params.isVerbose()) {
        logger::stream() << "Failed to merge because of low quality or impossible adjoining edge match" << std::endl; logger::flush();
        merge_failures++;
    }    
}

void PuzzleDisjointSet::finish() {
    if (merge_failures > 0) {
        std::cout << std::endl;
        logger::stream() << "Failed to merge because of overlap (" << ++merge_failures << " times)" << std::endl; logger::flush();
    }
}
int PuzzleDisjointSet::find(int a){
    int rep = a;
    while(sets[rep].representative != -1){
        rep = sets[rep].representative;
    }
    return rep;
}

std::vector<int> PuzzleDisjointSet::get_collection_sets() {
    return csets;
}

bool PuzzleDisjointSet::is_collection_set(int rep) {
    std::vector<int>::iterator i = std::find(csets.begin(), csets.end(), rep);
    return i != csets.end();
}

bool PuzzleDisjointSet::is_unmatched_set(int rep) {
    forest s = sets[rep];
    return (s.representative == -1 && s.locations.rows == 1 && s.locations.cols == 1);
}

int PuzzleDisjointSet::collection_set_count() {
    int result = csets.size();
    return result;
}

bool PuzzleDisjointSet::in_same_set(int a, int b){
    return (find(a) == find(b));
}


void PuzzleDisjointSet::rotate_ccw(int id,int times){
    int direction = times%4;
    switch (direction) {
        case 0: //Don't rotate
            return;
            break;
        case 1: //Rotate ccw 90 degrees
            cv::flip(sets[id].locations,sets[id].locations,1); //flip around y axis
            cv::transpose(sets[id].locations,sets[id].locations);
            cv::flip(sets[id].rotations,sets[id].rotations,1);
            cv::transpose(sets[id].rotations, sets[id].rotations);
            sets[id].rotations+=1;
            break;
        case 2: //rotate 180
            cv::flip(sets[id].locations,sets[id].locations,-1); //flip around both axises
            cv::flip(sets[id].rotations ,sets[id].rotations,-1); //flip around both axises
            sets[id].rotations+=2;
            break;
        case 3: //rotate cw 90 degrees
            cv::transpose(sets[id].locations,sets[id].locations);
            cv::flip(sets[id].locations,sets[id].locations,1); //flip around y axis
            cv::transpose(sets[id].rotations, sets[id].rotations);
            cv::flip(sets[id].rotations,sets[id].rotations,1);
            sets[id].rotations+=3;
            break;
        default://Should never get here!!!
            return;
            break;
    }
    //If there is no piece at the location, the rotation needs to be set back
    //to zero
    for(int i=0; i<sets[id].locations.size[0]; i++){
        for(int j=0; j<sets[id].locations.size[1]; j++){
            if(-1==sets[id].locations(i,j)) sets[id].rotations(i,j)=0;
        }
    }
    
    //basically rotations%4 (opencv does not have an operator to do this
    //Luckly the last 2 bits are all that is needed
    cv::bitwise_and(sets[id].rotations, 0x3, sets[id].rotations);
    return;
}


//Returns location of the number in the matrix
cv::Point PuzzleDisjointSet::find_location(cv::Mat_<int> m, int number ){
    for(int i = 0; i<m.size[0]; i++){
        for(int j=0; j<m.size[1]; j++){
            if(m(i,j)==number) return cv::Point(j,i);
        }
    }
    return cv::Point(0,0);
}


PuzzleDisjointSet::forest PuzzleDisjointSet::get(int id){
    return sets[id];
}

bool PuzzleDisjointSet::in_one_set(){
    return (1==set_count);
}
