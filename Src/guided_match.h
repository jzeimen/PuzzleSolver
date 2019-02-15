#ifndef GUIDED_MATCH_H
#define GUIDED_MATCH_H

#include "piece.h"
#include "params.h"

#define GM_COMMAND_X_CLOSE "x_close"
#define GM_COMMAND_NO "no"
#define GM_COMMAND_YES "yes"
#define GM_COMMAND_SHOW_SET "show_set"
#define GM_COMMAND_SHOW_ROTATION "show_rotation"
#define GM_COMMAND_WORK_ON_SET "work_on_set"
#define GM_COMMAND_MARK_BOUNDARY "mark_boundary"

/** @brief Display the guided match GUI for the given pieces/edges and waits for user input from the keyboard.
 * 
 * Returns a string value depending on which key the user pressed, see the #define's above for the possible values.
 * If the user presses the 'h' or '?' key, then usage information is displayed in the console output.

@param p1 A puzzle piece object
@param p2 Another puzzle piece object
@param e1 The edge number of p1
@param e2 The edge number of p2
@param user_params The user params object
*/
std::string guided_match(piece& p1, piece& p2, int e1, int e2, params& user_params);

#endif /* GUIDED_MATCH_H */

