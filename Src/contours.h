/* 
 *
 * Classes that participate in ordering the piece contours into rows (or columns)
 * so that the resulting pieces can be identified by their position within the 
 * input image. 
 * 
 */

#ifndef CONTOURS_H
#define CONTOURS_H

#include <map>
#include <vector>
#include "compat_opencv.h"
#include "params.h"

// Associate names such as "lrtb" (left->right, top->bottom) with the parameters required to
// order them via our row/colum partitioning scheme.
class piece_order {
public:
    std::string name; // "lrtb", etc.
    bool partition_rows; // true if initially partitioned into rows, false if columns
    bool partitions_asc; // true if partitions are sorted into ascending order, false if descending
    bool items_asc; // true if items in a partition are sorted into ascending order, false if descending
    piece_order(std::string name, bool partition_rows, bool partition_order_asc, bool item_order_asc);
    static piece_order* lookup(std::string name);
    static void names(std::vector<std::string>& namevec);
private: 
    static void init();    
    static std::map<std::string,piece_order*> items;
};

// Associate piece contour points and bounds so they can be sorted
class contour {
public:
    cv::Rect bounds;
    std::vector<cv::Point> points;
    int sort_factor;  // The value to sort on    
    contour(cv::Rect _bounds, std::vector<cv::Point> _points);
};

// A contour partition is created for each row (or column) found, so that
// we can sort the contours into the proper order.
class contour_partition {
public:
    int index; // The unsorted partition index
    int offset; // The offset in x (or y), used to order partition
    int order; // Order of the partition among other partitions after sorting by offset
    
    contour_partition(int index);
    
    void update_offset(int off);
};

// Contour manager 
class contour_mgr {
private:
    int container_width;
    int container_height;
    params& user_params;    
public:

    std::vector<contour> contours;
    
    contour_mgr(int _container_width, int _container_height, params& _user_params);
    
    void add_contour(cv::Rect _bounds, std::vector<cv::Point> _points);

    // Sort the contours so that the pieces end up being identified based on their position in the original image --
    // i.e., assuming the pieces are arranged in a grid in the image, then number them left to right going
    // from the top to the bottom.  The point of doing this is to provide some way to correlate hand-written piece
    // numbers with the numerical (text) output of this program and is especially helpful if a solution is found
    // but a final solution image can't be generated. The piece layout in the image does not need to be exact, 
    // but the differences in y of each row (or x of each column) must be less than the estimated piece size multiplied 
    // by the partition factor.  If "landscape" is true, then pieces are ordered top to bottom going left to right.
    void sort_contours();
    
};


#endif /* CONTOURS_H */

