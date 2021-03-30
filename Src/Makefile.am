bin_PROGRAMS = PuzzleSolver # gmtest
if USE_OPENCV4
OPENCV_CXXFLAGS = $(opencv4_CFLAGS)
OPENCV_LDDFLAGS = $(opencv4_LIBS)
else
OPENCV_CXXFLAGS = $(opencv_CFLAGS)
OPENCV_LDDFLAGS = $(opencv_LIBS)
endif
AM_CXXFLAGS = -std=c++11 $(OPENCV_CXXFLAGS) $(OPENMP_CFLAGS)
LDADD = $(OPENCV_LDDFLAGS) $(OPENMP_CFLAGS)
PuzzleSolver_SOURCES = adjust_corners.cpp contours.cpp edge.cpp guided_match.cpp image_viewer.cpp logger.cpp main.cpp params.cpp piece.cpp puzzle.cpp PuzzleDisjointSet.cpp utils.cpp
#gmtest_SOURCES = adjust_corners.cpp contours.cpp edge.cpp guided_match.cpp gmtest.cpp logger.cpp params.cpp piece.cpp utils.cpp
