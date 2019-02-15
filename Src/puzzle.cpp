//
//  puzzle.cpp
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/5/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#include "puzzle.h"

#include <sstream>
#include <vector>
#include <climits>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include "omp.h"
#include "compat_opencv.h"

#include "PuzzleDisjointSet.h"
#include "utils.h"
#include "contours.h"
#include "logger.h"
#include "guided_match.h"
#include "image_viewer.h"

typedef std::vector<cv::Mat> imlist;

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



puzzle::puzzle(params& _user_params) : user_params(_user_params) {
    pieces = extract_pieces();
    solved = false;
    if (user_params.isSavingEdges()) {
    	print_edges();
    }
    logger::stream() << "Extracted " << pieces.size() << " pieces" << std::endl;
    logger::flush();
}




void puzzle::print_edges(){
    
    cv::Scalar color = cv::Scalar(255);
    
    for(uint i =0; i<pieces.size(); i++){
        for(int j=0; j<4; j++){
            cv::Mat m = cv::Mat::zeros(500, 500, CV_8UC1 );

            std::vector<cv::Point> points = pieces[i].edges[j].get_translated_contour(200, 0);

            for (uint p = 0; p < points.size() -1 ; p++) {
                cv::line(m, points[p], points[p+1], color);
            }
            putText(m, pieces[i].edges[j].edge_type_to_s(), cv::Point(300,300),
                    cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, color, 1, COMPAT_CV_LINE_AA);

            utils::write_debug_img(user_params, m, "edge", pieces[i].get_id(), std::to_string(j));
        }
    }
}


std::vector<piece> puzzle::extract_pieces() {
    std::vector<piece> pieces;
    imlist color_images = utils::getImages(user_params.getInputDir());

    logger::stream() << "Extracting pieces..." << std::endl;    
    logger::flush();
    
    //Threshold the image, anything of intensity greater than 45 becomes white (255)
    //anything below becomes 0
//    imlist blured_images = blur(color_images, 7, 5);

    imlist bw;
    if(user_params.isUsingMedianFilter()){
        imlist blured_images = utils::median_blur(color_images, user_params.getMedianBlurKSize());
        bw = utils::color_to_bw(blured_images, user_params.getThreshold());
    } else{
        bw= utils::color_to_bw(color_images, user_params.getThreshold());
        utils::filter(bw,2);
    }

    uint piece_number = user_params.getInitialPieceId();
    

    //For each input image
    for(uint i = 0; i<color_images.size(); i++){

        char image_number_buf[80];
        sprintf(image_number_buf, "%03d", i+1);
        std::string image_number(image_number_buf);
        
        if (user_params.isSavingOriginals()) {
            utils::write_debug_img(user_params, bw[i],"original-bw", image_number);
            utils::write_debug_img(user_params, color_images[i], "original-color", image_number);
        }

        std::vector<std::vector<cv::Point> > found_contours;

        
        //This isn't used but the opencv function wants it anyways.
        std::vector<cv::Vec4i> hierarchy;

        //Need to clone b/c it will get modified
        cv::findContours(bw[i].clone(), found_contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

        
 
        //For each contour in that image
        //TODO: (In anticipation of the other TODO's Re-create the b/w image
        //    based off of the contour to eliminate noise in the layer mask

        contour_mgr contour_mgr(bw[i].size().width, bw[i].size().height, user_params); 

        for(uint j = 0; j < found_contours.size(); j++) {
            cv::Rect bounds =  cv::boundingRect(found_contours[j]);
            if(bounds.width < user_params.getEstimatedPieceSize() || bounds.height < user_params.getEstimatedPieceSize()) continue;
            
            contour_mgr.add_contour(bounds, utils::remove_duplicates(found_contours[j]));
        }

        contour_mgr.sort_contours();
        
        if (user_params.isVerifyingContours() || user_params.isSavingContours()) {
            std::vector<std::vector<cv::Point> > contours_to_draw;
            cv::Mat cmat = cv::Mat::zeros(bw[i].size().height, bw[i].size().width, CV_8UC3);    
            double font_scale = sqrt(bw[i].size().height * bw[i].size().width) / 1000;
            for (uint j = 0; j < contour_mgr.contours.size(); j++) {
                cv::Rect bounds = contour_mgr.contours[j].bounds;
                contours_to_draw.push_back(contour_mgr.contours[j].points);
                // Text indicating contour order within the image
                cv::putText(cmat, std::to_string(j+piece_number), cv::Point2f(bounds.x+bounds.width/2-(10.0*font_scale),bounds.y+bounds.height/2+(10.0*font_scale)),
                        cv::FONT_HERSHEY_COMPLEX_SMALL, font_scale, cv::Scalar(0, 255, 255), 1, COMPAT_CV_LINE_AA);                
            }

            cv::drawContours(cmat, contours_to_draw, -1, cv::Scalar(255,255,255), 2, 16);
            
            if (user_params.isVerifyingContours()) {
                if (i == 0) {
                    std::cout << "With focus on the contours image window:" << std::endl;
                    std::cout << "    press 't' to toggle between the contours and original image" << std::endl;
                    std::cout << "    press 'n' to advance to the next image" << std::endl;
                }
                show_images("contours-" + image_number, cmat, color_images[i]);
            }
            if (user_params.isSavingContours()) {
                utils::write_debug_img(user_params, cmat, "contours", image_number);
            }
        }
        
        // Uncomment to save a version of the original with the piece numbers overlayed
        /*
        if (user_params.isSavingOriginals()) {
            cv::Mat cmat = color_images[i].clone();
            double font_scale = sqrt(bw[i].size().height * bw[i].size().width) / 1000;
            for (uint j = 0; j < contour_mgr.contours.size(); j++) {
                cv::Rect bounds = contour_mgr.contours[j].bounds;
                // Text indicating contour order within the image
                cv::putText(cmat, std::to_string(j+1), cv::Point2f(bounds.x+bounds.width/2-(10.0*font_scale),bounds.y+bounds.height/2+(10.0*font_scale)),
                        cv::FONT_HERSHEY_COMPLEX_SMALL, font_scale, cv::Scalar(255,255,255), 2, cv::LINE_AA);                
            }

            write_debug_img(user_params, cmat, "numbered", image_number);
        }
        */
        
        for (uint j = 0; j < contour_mgr.contours.size(); j++) {
            int bordersize = 15;
            std::stringstream idstream;

            char id_buffer[80];
            snprintf(id_buffer, 80, "%03d-%03d-%04d", i+1, j+1, piece_number);
            std::string piece_id(id_buffer);
            
            cv::Rect bounds = contour_mgr.contours[j].bounds;
            std::vector<cv::Point> points = contour_mgr.contours[j].points;
            
            cv::Mat new_bw = cv::Mat::zeros(bounds.height+2*bordersize,bounds.width+2*bordersize,CV_8UC1);
            std::vector<std::vector<cv::Point> > contours_to_draw;
            contours_to_draw.push_back(utils::translate_contour(points, bordersize-bounds.x, bordersize-bounds.y));
            cv::drawContours(new_bw, contours_to_draw, -1, cv::Scalar(255), COMPAT_CV_FILLED);

            if (user_params.isSavingBlackWhite()) {
                utils::write_debug_img(user_params, new_bw, "bw", piece_id);
            }

            cv::Rect b2(bounds.x-3, bounds.y-3, bounds.width+6, bounds.height+6);
            cv::Mat color_roi = color_images[i](b2);
            cv::Mat mini_color = cv::Mat::zeros(bounds.height+2*bordersize,bounds.width+2*bordersize,CV_8UC3);
            color_roi.copyTo(mini_color(cv::Rect(bordersize-3,bordersize-3,b2.width,b2.height)));
            
            if (user_params.isSavingColor()) {
                utils::write_debug_img(user_params, mini_color, "color", piece_id);
            }
            cv::Mat mini_bw = new_bw;
            //Create a copy so it can't conflict.
            mini_color = mini_color.clone();
            mini_bw = mini_bw.clone();
            
            piece p(piece_number, piece_id, mini_color, mini_bw, user_params);
            pieces.push_back(p);
            
            piece_number += 1;
            
        }
    }
    
    for (std::vector<piece>::iterator i= pieces.begin(); i != pieces.end(); i++) {
        i->process();
    }

    return pieces;
}




void puzzle::fill_costs(){
    
    int no_edges = (int) pieces.size()*4;
    
    //TODO: use openmp to speed up this loop w/o blocking the commented lines below
//    omp_set_num_threads(4);
#pragma omp parallel for schedule(dynamic)
    for(int i =0; i<no_edges; i++){
        for(int j=i; j<no_edges; j++){
            match_score score;
            score.edge1 =(int) i;
            score.edge2 =(int) j;
            score.score = pieces[i/4].edges[i%4].compare3(pieces[j/4].edges[j%4]);
#pragma omp critical
{
            matches.push_back(score);
}
        }
    }
    std::sort(matches.begin(),matches.end(),match_score::compare);
}

void puzzle::auto_solve(PuzzleDisjointSet& p) {
    int output_id=0;
    
    
    std::vector<match_score>::iterator i= matches.begin();
    while(!p.in_one_set() && i!=matches.end() ){
        int p1 = i->edge1/4;
        int e1 = i->edge1%4;
        int p2 = i->edge2/4;
        int e2 = i->edge2%4;
        
        if (user_params.isSavingMatches()) {
            cv::Mat m = cv::Mat::zeros(500,500,CV_8UC1);
            std::stringstream out_file_name;
            out_file_name << user_params.getOutputDir() << "match" << output_id << "_" << pieces[p1].get_id() << "-" << e1 << "_" << pieces[p2].get_id() << "-" <<e2 << ".png";
            std::vector<std::vector<cv::Point> > contours;
            contours.push_back(pieces[p1].edges[e1].get_translated_contour(200, 0));
            contours.push_back(pieces[p2].edges[e2].get_translated_contour_reverse(200, 0));
            cv::polylines(m, contours, false, cv::Scalar(255));
            cv::imwrite(out_file_name.str(), m);
        }
        if (user_params.isVerbose()) {
            logger::stream() << "Attempting to merge: " << pieces[p1].get_id() << "-" << (e1+1) << " with: " << 
                    pieces[p2].get_id() << "-" << (e2+1) << ", score:" << i->score << " count: "  << output_id <<std::endl;
            logger::flush();
        }
        PuzzleDisjointSet::join_context c;
        p.init_join(c, p1, p2, e1, e2);
        p.compute_join(c);
        if (c.joinable) {
            p.complete_join(c);
        }
        i++;
        output_id += 1;
    }    
}

// Unlike auto_solve, in which the sets managed by PuzzleDisjointSet randomly coalesce during the solution phase,
// guided_solve attempts to help the human operator by keeping the number of matched sets down to a minimum
void puzzle::guided_solve(PuzzleDisjointSet& p) {
    int output_id=0;
    
    bool done = false;
    int work_on = -1;
    
    if (user_params.getWorkOnPiece() != -1) {
        work_on = user_params.getWorkOnPiece() - user_params.getInitialPieceId();
        
        if (work_on < 0 || work_on >= pieces.size()) {
            logger::stream() << "Error, 'work on' piece number " << user_params.getWorkOnPiece() << " is out of range.  Expected a value between " 
                    << user_params.getInitialPieceId() << " and " << (pieces.size() + user_params.getInitialPieceId() - 1) << std::endl;
            logger::flush();
            exit(1);
        }
    }
    
    
    while (!p.in_one_set()) {
        std::vector<match_score>::iterator i= matches.begin();
        while(!p.in_one_set() && i!=matches.end() ) {
            int p1 = i->edge1/4;
            int e1 = i->edge1%4;
            int p2 = i->edge2/4;
            int e2 = i->edge2%4;
            
            PuzzleDisjointSet::join_context c;
            p.init_join(c, p1, p2, e1, e2);
            if (!c.joinable || is_boundary_edge(p1, e1) || is_boundary_edge(p2, e2)) {
                i++;
                output_id += 1;
                continue;
            }
            
            if (work_on != -1) {
                if (work_on == c.rep_a) {
                    // no-op
                }
                else if (work_on == c.rep_b) {
                    p.init_join(c, p2, p1, e2, e1);
                }
                else {
                    i++;
                    output_id += 1;
                    continue;
                }
            }
            // Attempt to join if nothing has been joined yet (collection_set_count == 0), or if 
            // one of the two rep sets is a collection set and the other is unmatched.
            else if (p.collection_set_count() == 0 || (p.is_collection_set(c.rep_a) && p.is_unmatched_set(p2))) {
                // no-op
            }
            else if (p.is_collection_set(c.rep_b) && p.is_unmatched_set(p1)) {
                // swap order so that the collection set is in rep_a
                p.init_join(c, p2, p1, e2, e1);
            } 
            else {
                i++;
                output_id += 1;                
                continue;
            }
            
            p.compute_join(c);

            if (c.joinable) {
                std::string response = guide_match(c.a, c.b, c.how_a, c.how_b);
                if (response == GM_COMMAND_YES) {
                    p.complete_join(c);
                    break;
                }
                else if (response == GM_COMMAND_SHOW_SET) {
                    PuzzleDisjointSet::forest f = p.get(c.rep_a);
                    std::cout << set_to_string(f.locations, user_params.getInitialPieceId()) << std::endl;
                    continue;
                }
                else if (response == GM_COMMAND_SHOW_ROTATION) {
                    PuzzleDisjointSet::forest f = p.get(c.rep_a);
                    std::cout << set_to_string(f.rotations, 0) << std::endl;
                    continue;
                }   
                else if (response == GM_COMMAND_MARK_BOUNDARY) {
                    set_boundary_edge(c.a, c.how_a);
                    continue;
                }
                else if (response == GM_COMMAND_WORK_ON_SET) {
                    
                    std::cout << "Current matched groups IDs are: ";
                    for (uint j = 0; j < p.get_collection_sets().size(); j++) {
                        if (j > 0) {
                            std::cout << ", ";
                        }
                        std::cout << (p.get_collection_sets()[j] + user_params.getInitialPieceId());
                    }
                    std::cout << std::endl;
                    
                    int piece_number;
                    bool read_success = false;
                    
                    do {
                        std::cout << "Enter a piece number: " << std::flush;
                        std::cin >> piece_number;
                        
                        if (std::cin.fail()) {
                            std::cin.clear();
                            std::cin.ignore(999,'\n');
                            std::cout << "Invalid input" << std::endl;
                            continue;
                        }
                        
                        int work_on_id = piece_number - user_params.getInitialPieceId();
                        
                        if (work_on_id < 0 || work_on_id >= pieces.size()) {
                            std::cout << "Error, " << piece_number << " is out of range.  Expected a value between " 
                                    << user_params.getInitialPieceId() << " and " << (pieces.size() + user_params.getInitialPieceId() - 1) << std::endl;
                        }
                        else {
                            work_on = p.find(work_on_id);
                            std::cout << "Working on " << (work_on + user_params.getInitialPieceId());
                            if (work_on != work_on_id) {
                                std::cout << " (matched group for " << piece_number << ")";
                            }
                            std::cout << std::endl;
                            read_success = true;
                        }
                    } while (!read_success);
                    continue;
                }
                else if (response == GM_COMMAND_X_CLOSE) {
                    // Ignore
                    continue;
                }
            }
            i++;
            output_id += 1;                    
        }   
    }
}

bool match_check_function(void* data, int p1, int p2, int e1, int e2) {
    puzzle* p = (puzzle*)data;
    return p->check_match(p1, p2, e1, e2);
}

bool puzzle::check_match(int p1, int p2, int e1, int e2) {
    double cscore;
    double escore;
    double score = pieces[p1].edges[e1].compare3(pieces[p2].edges[e2], cscore, escore);
    if (user_params.isVerbose()) {
        std::cout << "check_match(" << (p1+user_params.getInitialPieceId()) << ", " << (p2+user_params.getInitialPieceId()) 
                << ", " << e1 << ", " << e2 << ")=" << cscore << " / " << escore << std::endl;
    }
    if (score == DBL_MAX || cscore > user_params.getCscoreLimit() || escore > user_params.getEscoreLimit()) {
        return false;
    }
    return true;
}

//Solves the puzzle
void puzzle::solve(){
    
    load_guided_matches();
    load_boundary_edges();
    
    PuzzleDisjointSet p(user_params, pieces.size(), match_check_function, this);
    // PuzzleDisjointSet p(user_params, pieces.size(), NULL, NULL);
    
    if (!user_params.isGuidedSolution()) {
        auto_solve(p);
    }
    else {
        guided_solve(p);
    }

    p.finish();
    
    if(p.in_one_set()){
        logger::stream() << "Possible solution found" << std::endl;
        logger::flush();
        solved = true;
        PuzzleDisjointSet::forest f = p.get(p.find(1));
        solution = f.locations;
        solution_rotations = f.rotations;
        
        for(int i =0; i<solution.size[0]; i++){
            for(int j=0; j<solution.size[1]; j++){
                int piece_number = solution(i,j);
                pieces[piece_number].rotate(4-solution_rotations(i,j));
            }
        }   
    }
}

std::string get_boundary_edges_filename(params& user_params) {
    return user_params.getOutputDir() + "boundary-edges.dat";
}

void puzzle::load_boundary_edges() {
    std::string filename = get_boundary_edges_filename(user_params);
    std::ifstream istream;
    istream.open(filename, std::ifstream::in);
    if (istream.fail()) {
        return;
    }

    std::string date;
    std::string id;
    
    while (true) {

        istream >> date;
        istream >> id;
        if (istream.eof()) {
            break;
        }
        
        boundary_edges[id] = "yes";
    }
    
    istream.close();
}

std::string get_boundary_edge_id(int initial_piece_id, int p1, int e1) {
    int id_p1 = p1 + initial_piece_id;
    int id_e1 = (e1+1);

    std::stringstream idstream;
    idstream << id_p1 << "-" << id_e1;
    return idstream.str();
}

void puzzle::set_boundary_edge(int p1, int e1) {
    
    std::string id = get_boundary_edge_id(user_params.getInitialPieceId(), p1, e1);
    boundary_edges[id] = "yes";
    

    std::string filename = get_boundary_edges_filename(user_params);
    std::ofstream ostream;
    ostream.open(filename, std::ofstream::out | std::ofstream::app);
    if (ostream.fail()) {
        std::cerr << "Failed to open " << filename << " for writing" << std::endl;
        exit(1);
    }
    
    std::time_t time = std::time(NULL);
    std::tm tm = *std::localtime(&time);
    ostream << std::put_time(&tm, "%a_%F_%T") << " " << id << "\n" << std::flush;
    ostream.close();    
}

bool puzzle::is_boundary_edge(int p1, int e1) {
    std::string id = get_boundary_edge_id(user_params.getInitialPieceId(), p1, e1);    
    std::map<std::string,std::string>::iterator it = boundary_edges.find(id);
    return it != boundary_edges.end();
}

std::string get_guided_matches_filename(params& user_params) {
    return user_params.getOutputDir() + "guided-matches.dat";
}

void puzzle::load_guided_matches() {
    std::string filename = get_guided_matches_filename(user_params);
    std::ifstream istream;
    istream.open(filename, std::ifstream::in);
    if (istream.fail()) {
        return;
    }

    std::string dateField;
    std::string isMatchField;
    std::string piecePairId;
    
    while (true) {

        istream >> dateField;
        istream >> isMatchField;
        istream >> piecePairId;
        if (istream.eof()) {
            break;
        }
        
        guided_matches[piecePairId] = isMatchField;
    }
    
    istream.close();
}

// Returns a string identifying the piece-edge paring
std::string get_match_id(int initial_piece_id, int p1, int p2, int e1, int e2) {
    // Create the ID with the lower value piece number appearing first.
    int id_p1;
    int id_e1;
    int id_p2;
    int id_e2;
    
    if (p1 < p2) {
        id_p1 = p1 + initial_piece_id;
        id_e1 = (e1+1);
        id_p2 = p2 + initial_piece_id;
        id_e2 = (e2+1);
    } else {
        id_p1 = p2 + initial_piece_id; 
        id_e1 = (e2+1);
        id_p2 = p1 + initial_piece_id;
        id_e2 = (e1+1);                
    }

    std::stringstream idstream;
    idstream << id_p1 << "-" << id_e1 << "-" << id_p2 << "-" << id_e2;
    return idstream.str();    
}

std::string puzzle::guide_match(int p1, int p2, int e1, int e2) {
    
    std::string id = get_match_id(user_params.getInitialPieceId(), p1, p2, e1, e2);
    
    std::map<std::string,std::string>::iterator it = guided_matches.find(id);
    if (it != guided_matches.end()) {
        return it->second;
    }
    
    if (!user_params.isGuidedSolution()) {
        return "yes";  // "yes" results in the default automatic solution behavior
    }
    
    double cscore;
    double escore;
    double score = pieces[p1].edges[e1].compare3(pieces[p2].edges[e2], cscore, escore);
    
    std::cout << "Does piece " << (p1 + user_params.getInitialPieceId()) 
            << " fit to " << (p2 + user_params.getInitialPieceId()) 
            << " (scores: " << cscore << " / " << escore << ")"
            << " ? " << std::flush;
    
    std::string response = guided_match(pieces[p1], pieces[p2], e1, e2, user_params);
    std::cout << response << std::endl;

    if (response != "yes" &&  response != "no") {
        return response;
    }
    guided_matches[id] = response;
    

    std::string filename = get_guided_matches_filename(user_params);
    std::ofstream ostream;
    ostream.open(filename, std::ofstream::out | std::ofstream::app);
    if (ostream.fail()) {
        std::cerr << "Failed to open " << filename << " for writing" << std::endl;
        exit(1);
    }
    
    std::time_t time = std::time(NULL);
    std::tm tm = *std::localtime(&time);
    ostream << std::put_time(&tm, "%a_%F_%T") << " " << response << " " << id << "\n" << std::flush;
    ostream.close();
    return response;
}

std::string puzzle::set_to_string(cv::Mat_<int> set, int offset) {
    std::stringstream stream;
    
    int width = 2;
    int max_id = pieces.size() + user_params.getInitialPieceId() -1;
    if (max_id > 99) {
        width = 3;
    } 
    else if (max_id > 999) {
        width = 4;
    }
    
    for(int row = 0; row < set.rows; ++row) {
        int* p = (int*)set.ptr(row);
        for(int col = 0; col < set.cols; ++col) {
            int value = *p++;
            if (value >= 0) {
                stream << std::setw(width) << (value + offset);
            }
            else {
                stream << std::setw(width) << "";
            }
            stream << ", ";
        }
        stream << std::endl;
    }
    
    return stream.str();    
}

// Generates and displays the solution as text (grid of piece IDs).  The text is also saved to 
// <solution>.txt in the output directory.  The numbers are 1-based instead of zero-based so
// that they correspond to the piece IDs.
void puzzle::save_solution_text() {
    if(!solved) solve();
    
    std::string solution_text = set_to_string(solution, user_params.getInitialPieceId());
    logger::stream() << solution_text << std::endl;
    logger::flush();
    
    logger::stream() << "\nrotations:\n" << set_to_string(solution_rotations, 0) << std::endl;
      
}

std::string puzzle::get_solution_image_pathname() {
    return user_params.getOutputDir() + user_params.getSolutionFileBasename() + ".png";
}



//Saves an image of the representation of the puzzle.
//only really works when there are no holes
//TODO: fail when puzzle is in configurations that are not possible i.e. holes
void puzzle::save_solution_image(){
    if(!solved) solve();
    
    
    //Use get affine to map points...
    int out_image_size = 6000;
    cv::Mat out_image(out_image_size,out_image_size,CV_8UC3, cv::Scalar(200,50,3));
    int border = 10;
    
    cv::Point2f ** points = new cv::Point2f*[solution.size[0]+1];
    for(int i = 0; i < solution.size[0]+1; ++i)
        points[i] = new cv::Point2f[solution.size[1]+1];
    bool failed=false;
    
    logger::stream() << "Saving image..." << std::endl;
    logger::flush();
    for(int i=0; i<solution.size[0];i++){
        for(int j=0; j<solution.size[1]; j++){
            int piece_number = solution(i,j);
            logger::stream() << std::setfill(' ') << std::setw(2) << "." << std::flush; logger::flush();

            if(piece_number ==-1){
                failed = true;
                // break;
                continue;
            }
            float x_dist =(float) cv::norm(pieces[piece_number].get_corner(0)-pieces[piece_number].get_corner(3));
            float y_dist =(float) cv::norm(pieces[piece_number].get_corner(0)-pieces[piece_number].get_corner(1));
            std::vector<cv::Point2f> src;
            std::vector<cv::Point2f> dst;
            
            if(i==0 && j==0){
                points[i][j] = cv::Point2f(border,border);
            }
            if(i==0){
                points[i][j+1] = cv::Point2f(points[i][j].x+border+x_dist,border);
            }
            if(j==0){
                points[i+1][j] = cv::Point2f(border,points[i][j].y+border+y_dist);
            }
            
            dst.push_back(points[i][j]);
            dst.push_back(points[i+1][j]);
            dst.push_back(points[i][j+1]);
            src.push_back(pieces[piece_number].get_corner(0));
            src.push_back(pieces[piece_number].get_corner(1));
            src.push_back(pieces[piece_number].get_corner(3));

            //true means use affine transform
            cv::Mat a_trans_mat = cv::estimateRigidTransform(src, dst,true);
            cv::Mat_<double> A = a_trans_mat;
            
            //Lower right corner of each piece
            cv::Point2f l_r_c = pieces[piece_number].get_corner(2);
            
            //Doing my own matrix multiplication
            points[i+1][j+1] = cv::Point2f((float)(A(0,0)*l_r_c.x+A(0,1)*l_r_c.y+A(0,2)),(float)(A(1,0)*l_r_c.x+A(1,1)*l_r_c.y+A(1,2)));
            
            
            
            cv::Mat layer;
            cv::Mat layer_mask;
            
            int layer_size = out_image_size;
            
            cv::warpAffine(pieces[piece_number].full_color, layer, a_trans_mat, cv::Size2i(layer_size,layer_size),cv::INTER_LINEAR,cv::BORDER_TRANSPARENT);
            cv::warpAffine(pieces[piece_number].bw, layer_mask, a_trans_mat, cv::Size2i(layer_size,layer_size),cv::INTER_NEAREST,cv::BORDER_TRANSPARENT);
            
            layer.copyTo(out_image(cv::Rect(0,0,layer_size,layer_size)), layer_mask);
            
        }
        logger::stream() << std::endl; logger::flush();

    }
    if(failed){
        logger::stream() << "Failed, only partial image generated" << std::endl; logger::flush();
    }

    cv::Mat final_out_image;
    
    utils::autocrop(out_image, final_out_image);
    cv::imwrite(get_solution_image_pathname(), final_out_image);
    
    

    for(int i = 0; i < solution.size[0]+1; ++i)
        delete points[i];
    delete[] points;
    
}


void puzzle::show_solution_image() {
    cv::Mat solution_image = cv::imread(get_solution_image_pathname());
    std::cout << "With focus on the solution image window:" << std::endl;
    std::cout << "    press 'r' one or more times to rotate the solution image by 90 degrees" << std::endl;
    std::cout << "    press 'q' to quit/exit" << std::endl;
    show_image("solution", solution_image);
}
