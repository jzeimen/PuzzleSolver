//
//  main.cpp
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/4/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string.h>
#include <cassert>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <libgen.h>

#include "cxxopts.hpp"
#include "logger.h"
#include "params.h"
#include "puzzle.h"
#include "utils.h"
#include "contours.h"
#include "config.h"


class demo {
public:
    std::string name;
    std::string inputDir;
    int estimated_piece_size;
    int threshold;
    bool filter;
    std::string comment;

    demo(std::string name, std::string inputDir, int estimated_piece_size, int threshold, bool filter, std::string comment) :
        name(name), inputDir(inputDir), estimated_piece_size(estimated_piece_size), 
        threshold(threshold), filter(filter), comment(comment) 
    {
    }

};

void register_demo(std::map<std::string,demo*> &demos, std::string name, std::string inputDir, int estimated_piece_size, int threshold, bool filter, std::string comment)
{
    demo* demoptr = new demo(name, inputDir, estimated_piece_size, threshold, filter, comment);
    demos[demoptr->name] = demoptr;
}

void demo_help(std::map<std::string,demo*> &demos)
{
    std::cout << std::setw(24) << std::left << "DEMO NAME" << "   COMMENT" << std::endl;
    for (std::map<std::string,demo*>::iterator it=demos.begin(); it!=demos.end(); ++it) {
        demo* d = it->second;
        std::cout << std::setw(24) << std::left << d->name << " : " << d->comment << std::endl;
    }
}

int main(int argc, char * argv[])
{
    params user_params;
    
    std::map<std::string,demo*> demos;
    register_demo(demos, "toy-story-color", "Toy Story", 200, 22, true, "48 pieces, --estimated-size 200 --threshold 22 --filter");
    register_demo(demos, "toy-story-back", "Toy Story back", 200, 50, false, "48 pieces, --estimated-size 200 --threshold 50");
    register_demo(demos, "angry-birds-color", "Angry Birds/color", 300, 30, false, "24 pieces, --estimated-size 300 --threshold 30");
    register_demo(demos, "angry-birds-scanner-open", "Angry Birds/Scanner Open", 300, 30, false, "24 pieces, --estimated-size 300 --threshold 30");
    register_demo(demos, "horses", "horses", 380, 50, false, "104 pieces, --estimated-size 380 --threshold 50");
    register_demo(demos, "horses-numbered", "horses numbered", 380, 50, false, "104 pieces, --estimated-size 380 --threshold 50");

    cxxopts::Options options("PuzzleSolver", std::string("Version: ") + std::string(VERSION) + std::string("\nSolve jigsaw puzzles using the shapes of the piece edges.\n"));

    options.add_options()
      ("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value("false"))
      ("h,help", "Display this help message")
      ("s,solve", "Solve the puzzle after processing the input images and extracting pieces and edges", cxxopts::value<bool>()->default_value("false"))
      ("g,guided", "Enable interactive solution mode", cxxopts::value<bool>()->default_value("false"))  
      ("w,work-on", "In guided solution mode, start solving with this piece number", cxxopts::value<int>()->default_value("-1"))
      ("dont-solve", "Skip finding the solution (e.g., for a demo which normally implies --solve)", cxxopts::value<bool>()->default_value("false"))
      ("n,solution-name", "Basename for solution text/image/log files written to the output directory", cxxopts::value<std::string>()->default_value("solution"))      
      ("e,estimated-size", "Estimated piece size", cxxopts::value<uint>()->default_value("200"))
      ("t,threshold", "Threshold value used when converting color images to b&w.  Min: 0, max: 255.", cxxopts::value<uint>()->default_value("30"))
      ("f,filter", "Use filter() instead of median_filter()", cxxopts::value<bool>()->default_value("false"))
      ("m,median-blur-ksize", "Median blur ksize value. Must be odd and greater than 1, e.g.: 3, 5, 7 ...", cxxopts::value<uint>()->default_value("5"))
      ("r,verify-contours", "Show the contours found in each input image", cxxopts::value<bool>()->default_value("false"))
      ("i,initial-piece-id", "Identify pieces starting with this number", cxxopts::value<uint>()->default_value("1"))            
      ("o,order", "Order of pieces in the input images", cxxopts::value<std::string>()->default_value("lrtb"))
      ("p,partition", "Piece-ordering partition factor for adjusting behavior of --order", cxxopts::value<float>()->default_value("1.0"))                
      ("b,corners-blocksize", "Block size to use when finding corners", cxxopts::value<uint>()->default_value("25"))            
      ("c,corners-quality", "Corner quality warning threshold", cxxopts::value<uint>()->default_value("300"))              
      ("a,adjust-corners","Show GUI corner adjuster for each piece where its corner quality exceeds the corners quality threshold", cxxopts::value<bool>()->default_value("false"))
      ("l,scale","Scale factor for images shown in GUI windows",  cxxopts::value<float>()->default_value("1.0"))
      ("cscore-limit","Limit of cscore values auto accepted as matches", cxxopts::value<float>()->default_value("125.0"))            
      ("escore-limit","Limit of escore values auto accepted as matches", cxxopts::value<float>()->default_value("4000.0"))                        
      ("save-all", "Save all images (originals, contours, b&w, color, corners, edges)", cxxopts::value<bool>()->default_value("false"))
      ("save-originals", "Save original images", cxxopts::value<bool>()->default_value("false"))                        
      ("save-contours", "Save contour images", cxxopts::value<bool>()->default_value("false"))            
      ("save-bw", "Save black&white piece images", cxxopts::value<bool>()->default_value("false"))                        
      ("save-color", "Save color piece images", cxxopts::value<bool>()->default_value("false"))                                    
      ("save-corners", "Save piece images showing corner locations", cxxopts::value<bool>()->default_value("false"))                                                
      ("save-edges", "Save images for each piece edge", cxxopts::value<bool>()->default_value("false")) 
      ("save-matches", "Save images for each pair of matched edges (in auto solve mode only)", cxxopts::value<bool>()->default_value("false"))             
      ("d,demo","Solve a named demo puzzle.  See below for a list of demos.", cxxopts::value<std::string>()) 

      ("positional","Positional parameters", cxxopts::value<std::vector<std::string>>())
      ;

    options.parse_positional({"positional"});
    options.positional_help("<directory containing input images> <output directory>");
    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
    	std::cout << options.help({"", "Group"}) << std::endl;
        demo_help(demos);        
    	exit(0);
    }
    
    bool is_demo = result.count("demo") > 0;
    if (is_demo) {
        std::string demo_name = result["demo"].as<std::string>();
        std::map<std::string,demo*>::iterator it = demos.find(demo_name);
        if (it == demos.end()) {
            std::cout << "ERROR: Unknown demo name: " << demo_name << std::endl;
            std::cout << "Use --help to get the list of demo names" << std::endl;            
            exit(1);
        }
        demo* demoptr = it->second;  

        // Resolve the PuzzleSolver home dir (parent of 'Scans'), by assuming the PuzzleSolver exe file is in the source directory.
        char* dirnamebuf = realpath(argv[0], NULL);
        char* puzzleSolverHome=dirname(dirname(dirnamebuf));
        user_params.setInputDir(std::string(puzzleSolverHome) + "/Scans/" + demoptr->inputDir);
        free(dirnamebuf);
        user_params.setOutputDir("/tmp/"+demoptr->name);
        user_params.setSolving(true);        
        user_params.setEstimatedPieceSize(demoptr->estimated_piece_size);
        user_params.setThreshold(demoptr->threshold);
        user_params.setUsingMedianFilter(!demoptr->filter);

        // Allow some demo default values to be explicity overridden
        if (result.count("estimated-size")) {
            user_params.setEstimatedPieceSize(result["estimated-size"].as<uint>());            
        }
        if (result.count("threshold")) {
            user_params.setThreshold(result["threshold"].as<uint>());
        }
        if (result.count("filter")) {
            user_params.setUsingMedianFilter(!result["filter"].as<bool>());
        }
    }
    else {
        if (result.count("positional") != 2)
        {
            std::cout << "ERROR: check positional args" << std::endl << std::endl;
            std::cout << options.help({"", "Group"}) << std::endl;
            exit(1);
        }
        auto& positional = result["positional"].as<std::vector<std::string>>();
        user_params.setInputDir(positional[0]);
        user_params.setOutputDir(positional[1]);
        user_params.setSolving(result["solve"].as<bool>());
        user_params.setEstimatedPieceSize(result["estimated-size"].as<uint>());
        user_params.setThreshold(result["threshold"].as<uint>());
        user_params.setUsingMedianFilter(!result["filter"].as<bool>());        
    }
    
    std::string order = result["order"].as<std::string>();
    if (piece_order::lookup(order) == NULL) {
        std::cout << "ERROR: Order '" << order << "' is invalid, expected one of: ";
        std::vector<std::string> nv;
        piece_order::names(nv);
        for (int i = 0; i < nv.size(); i++) {
            std::cout << nv[i];
            if (i < (nv.size() - 1)) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
        
        exit(1);
    }

    bool guided = result["guided"].as<bool>();
    user_params.setGuidedSolution(guided);
    if (guided) {
        user_params.setSolving(true);
    }
    if (result.count("dont-solve")) {
        user_params.setSolving(false);
    }
    user_params.setVerbose(result["verbose"].as<bool>());
    user_params.setSolutionFileBasename(result["solution-name"].as<std::string>());
    user_params.setWorkOnPiece(result["work-on"].as<int>());
    user_params.setMedianBlurKSize(result["median-blur-ksize"].as<uint>());
    user_params.setPieceOrder(result["order"].as<std::string>());
    user_params.setInitialPieceId(result["initial-piece-id"].as<uint>());
    user_params.setPartitionFactor(result["partition"].as<float>());
    user_params.setFindCornersBlockSize(result["corners-blocksize"].as<uint>());
    user_params.setMinCornersQuality(result["corners-quality"].as<uint>());  
    user_params.setAdjustingCorners(result["adjust-corners"].as<bool>());
    user_params.setGuiScale(result["scale"].as<float>());
    user_params.setCscoreLimit(result["cscore-limit"].as<float>());
    user_params.setEscoreLimit(result["escore-limit"].as<float>());  
    user_params.setVerifyingContours(result["verify-contours"].as<bool>());
    user_params.setSaveAll(result["save-all"].as<bool>());
    user_params.setSavingOriginals(result["save-originals"].as<bool>());    
    user_params.setSavingContours(result["save-contours"].as<bool>());        
    user_params.setSavingBlackWhite(result["save-bw"].as<bool>());    
    user_params.setSavingColor(result["save-color"].as<bool>());    
    user_params.setSavingCorners(result["save-corners"].as<bool>());    
    user_params.setSavingEdges(result["save-edges"].as<bool>());        
    user_params.setSavingMatches(result["save-matches"].as<bool>()); 

      
    mkdir(user_params.getOutputDir().c_str(), 0775);
    
    std::stringstream logfilename;
    logfilename << user_params.getOutputDir() << user_params.getSolutionFileBasename() << ".log";
    logger::filename(logfilename.str());

    logger::stream() << user_params.to_string() << std::endl; logger::flush();
    logger::stream() << "Starting..." << std::endl; logger::flush();
    timeval time;
    gettimeofday(&time, NULL);
    long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
    long inbetween_millis = millis;


    puzzle puzzle(user_params);

    gettimeofday(&time, NULL);
    logger::stream() << std::endl << "time to initialize:"  << (((time.tv_sec * 1000) + (time.tv_usec / 1000))-inbetween_millis)/1000.0 << std::endl;
    logger::flush();
    inbetween_millis = ((time.tv_sec * 1000) + (time.tv_usec / 1000));
    
    if (!user_params.isSolving()) {
        return 0;
    }
    
    logger::stream() << "Finding edge costs..." << std::endl;
    logger::flush();
    puzzle.fill_costs();
    gettimeofday(&time, NULL);
    logger::stream() << std::endl << "time to fill edge costs:"  << (((time.tv_sec * 1000) + (time.tv_usec / 1000))-inbetween_millis)/1000.0 << std::endl;
    logger::flush();
    inbetween_millis = ((time.tv_sec * 1000) + (time.tv_usec / 1000));
    
    puzzle.solve();
    gettimeofday(&time, NULL);
    logger::stream() << std::endl << "time to solve:"  << (((time.tv_sec * 1000) + (time.tv_usec / 1000))-inbetween_millis)/1000.0 << std::endl;
    logger::flush();
    inbetween_millis = ((time.tv_sec * 1000) + (time.tv_usec / 1000));
    puzzle.save_solution_text();
    puzzle.save_solution_image();
    gettimeofday(&time, NULL);
    logger::stream() << std::endl << "Time to draw:"  << (((time.tv_sec * 1000) + (time.tv_usec / 1000))-inbetween_millis)/1000.0 << std::endl;
    logger::flush();
    
    
    gettimeofday(&time, NULL);
    logger::stream() << std::endl << "total time:"  << (((time.tv_sec * 1000) + (time.tv_usec / 1000))-millis)/1000.0 << std::endl;
    logger::flush();
    
    puzzle.show_solution_image();
    
    return 0;
}


