//
//  PuzzleDisjointSet.cpp
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/12/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#include "PuzzleDisjointSet.h"
#include <algorithm>

PuzzleDisjointSet::PuzzleDisjointSet(int number){
    set_count=0;
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


bool PuzzleDisjointSet::join_sets(int a, int b, int how_a, int how_b){
    int rep_a = find(a);
    int rep_b = find(b);
    if(rep_a==rep_b) return  false; //Already in same set...
    


    
    //We need A to have its adjoining edge to be to the right, position 2
    // meaning if its rotation was 0 it would need to be rotated by 2
    cv::Point loc_of_a = find_location(sets[rep_a].locations, a);
    int rot_a = sets[rep_a].rotations(loc_of_a);
    int to_rot_a = (6 - how_a - rot_a)%4;
    rotate_ccw(rep_a, to_rot_a);
    
    //We need B to have its adjoinign edge to the left, position 0
    //if its position was 0, 
    cv::Point loc_of_b = find_location(sets[rep_b].locations, b);
    int rot_b = sets[rep_b].rotations(loc_of_b);
    int to_rot_b = (4-rot_b-how_b)%4; //incorrect!
    rotate_ccw(rep_b, to_rot_b);
    

    //figure out the size of the new Mats
    loc_of_a = find_location(sets[rep_a].locations, a);
    cv::Mat::MSize size_of_a = sets[rep_a].locations.size;
    loc_of_b = find_location(sets[rep_b].locations, b);
    cv::Mat::MSize size_of_b = sets[rep_b].locations.size;
    
    int width = std::max(size_of_a[1], loc_of_a.x - loc_of_b.x +1 +size_of_b[1]) - std::min(0, loc_of_a.x-loc_of_b.x +1);
    int height = std::max(size_of_a[0], loc_of_a.y - loc_of_b.y +size_of_b[0]) - std::min(0, loc_of_a.y-loc_of_b.y);
    
    
    //place old A and B into the new Mats of same size
    
    cv::Mat_<int> new_a_locs(height, width, -1);
    cv::Mat_<int> new_b_locs(height, width, -1);
    cv::Mat_<int> new_a_rots(height, width, 0);
    cv::Mat_<int> new_b_rots(height, width, 0);
    
    
    
    
    int ax_offset = std::abs(std::min(0, loc_of_a.x-loc_of_b.x+1));
    int ay_offset = std::abs(std::min(0, loc_of_a.y-loc_of_b.y));
    
    
    //B is moving when it isn't supposed to...
    int bx_offset = -(loc_of_b.x -( loc_of_a.x+ax_offset+1));
    int by_offset = -(loc_of_b.y -(loc_of_a.y+ay_offset));

    
    sets[rep_a].locations.copyTo(new_a_locs(cv::Rect(ax_offset,ay_offset,size_of_a[1],size_of_a[0])));
    sets[rep_a].rotations.copyTo(new_a_rots(cv::Rect(ax_offset,ay_offset,size_of_a[1],size_of_a[0])));
    sets[rep_b].locations.copyTo(new_b_locs(cv::Rect(bx_offset,by_offset,size_of_b[1],size_of_b[0])));
    sets[rep_b].rotations.copyTo(new_b_rots(cv::Rect(bx_offset,by_offset,size_of_b[1],size_of_b[0])));

    


    //check for overlap while combining...
    for(int i = 0; i<new_a_locs.size[0]; i++){
        for(int j=0; j<new_a_locs.size[1]; j++){
            //IF both have a real value for a piece, it becomes impossible, reject
            if(new_a_locs(i,j) != -1 && new_b_locs(i,j)!= -1){
                return false;
            }
            
            if(new_a_locs(i,j) == -1 && new_b_locs(i,j)!=-1){
                new_a_locs(i,j) = new_b_locs(i,j);
            }
            
            if(new_a_locs(i,j) == -1) new_a_rots(i,j)=0;
            if(new_b_locs(i,j) == -1) new_b_rots(i,j)=0;
            
        }
    }
    
    //Add the rotation matricies to each other.
    new_a_rots = new_a_rots+new_b_rots;
   
    
    //Set the new representative a, to have this Mat
    sets[rep_a].locations = new_a_locs;
    sets[rep_a].rotations = new_a_rots;
    set_count--;
    sets[rep_b].representative = rep_a;
    return true; //Everything seems ok if it got this far
}
int PuzzleDisjointSet::find(int a){
    int rep = a;
    while(sets[rep].representative != -1){
        rep = sets[rep].representative;
    }
    return rep;
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
            //basically rotations = (rotations+1)%4
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
    //basically moding by 4
    cv::bitwise_and(sets[id].rotations, 0x3, sets[id].rotations);
    return;
}



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
    std::cout << "count:" << set_count << std::endl;
    return (1==set_count);
}