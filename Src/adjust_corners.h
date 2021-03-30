#ifndef ADJUST_CORNERS_H
#define ADJUST_CORNERS_H

/** @brief Display a GUI which allows the corners of a piece to be manually adjusted.

Returns true if the corners are moved to new locations, false otherwise.
Keyboard presses on 'n', 'Return', or 'Enter' will end the editing operation, causing this method to return.
A keyboard press on 'r' will reset the editor to its original, unedited state.
The '+' and '-' keys can be used to increase/decrease the display scale factor. 

@param window_name The name to use for the highgui window.
@param image The image of the puzzle piece
@param scale_factor The image shown in the editor is scaled up by this factor to make editing easier
@param original_corners The unedited (input) corners
@param edited_corners If the corners are moved, this vector is populated with the new corner locations.
@param verbose If true, some information will be sent to the console during the edit process.
*/
bool adjust_corners(std::string& window_name, cv::Mat& image, float scale_factor, std::vector<cv::Point2f>& original_corners, std::vector<cv::Point2f>& edited_corners, bool verbose);

#endif /* ADJUST_CORNERS_H */

