/*
 * Preprocessor defines that allow PuzzleSolver to be compatible with OpenCV 2.x, 3.x, and 4.x.
 */

#ifndef COMPAT_OPENCV_H
#define COMPAT_OPENCV_H

#include "opencv2/opencv.hpp"
#include "opencv2/core/version.hpp"

#ifdef CV_VERSION_EPOCH
#define OPENCV_VERSION_MAJOR CV_VERSION_EPOCH
#define OPENCV_VERSION_MINOR CV_VERSION_MAJOR
#else
#define OPENCV_VERSION_MAJOR CV_VERSION_MAJOR
#define OPENCV_VERSION_MINOR CV_VERSION_MINOR
#endif

#if OPENCV_VERSION_MAJOR == 2
#include <opencv2/core/types_c.h>
#else
#include <opencv2/core/types.hpp>
#endif



#if OPENCV_VERSION_MAJOR == 2

#define COMPAT_CV_MAT_SIZE cv::Mat::MSize
#define COMPAT_CV_BGR2GRAY CV_BGR2GRAY
#define COMPAT_CV_MORPH_TYPE_OPEN CV_MOP_OPEN
#define COMPAT_CV_MORPH_TYPE_CLOSE CV_MOP_CLOSE
#define COMPAT_CV_TERM_CRITERIA_EPS CV_TERMCRIT_EPS
#define COMPAT_CV_TERM_CRITERIA_MAX_ITER  CV_TERMCRIT_ITER
#define COMPAT_CV_CONTOURS_MATCH_I2 CV_CONTOURS_MATCH_I2
#define COMPAT_CV_FILLED CV_FILLED
#define COMPAT_CV_LINE_AA CV_AA
#define ROTATE_90_CLOCKWISE 0 //!<Rotate 90 degrees clockwise
#define ROTATE_180 1 //!<Rotate 180 degrees clockwise
#define ROTATE_90_COUNTERCLOCKWISE 2 //!<Rotate 270 degrees clockwise
#define compat_cv_rotate cv_rotate

#elif OPENCV_VERSION_MAJOR == 3

#define COMPAT_CV_MAT_SIZE cv::MatSize
#define COMPAT_CV_BGR2GRAY CV_BGR2GRAY
#define COMPAT_CV_MORPH_TYPE_OPEN CV_MOP_OPEN
#define COMPAT_CV_MORPH_TYPE_CLOSE CV_MOP_CLOSE
#define COMPAT_CV_TERM_CRITERIA_EPS CV_TERMCRIT_EPS
#define COMPAT_CV_TERM_CRITERIA_MAX_ITER  CV_TERMCRIT_ITER
#define COMPAT_CV_CONTOURS_MATCH_I2 cv::CONTOURS_MATCH_I2
#define COMPAT_CV_FILLED cv::FILLED
#define COMPAT_CV_LINE_AA cv::LINE_AA
#define compat_cv_rotate cv::rotate

#elif OPENCV_VERSION_MAJOR == 4

#define COMPAT_CV_MAT_SIZE cv::MatSize
#define COMPAT_CV_BGR2GRAY cv::COLOR_BGR2GRAY
#define COMPAT_CV_MORPH_TYPE_OPEN cv::MorphTypes::MORPH_OPEN
#define COMPAT_CV_MORPH_TYPE_CLOSE cv::MorphTypes::MORPH_CLOSE
#define COMPAT_CV_TERM_CRITERIA_EPS cv::TermCriteria::EPS
#define COMPAT_CV_TERM_CRITERIA_MAX_ITER  cv::TermCriteria::MAX_ITER
#define COMPAT_CV_CONTOURS_MATCH_I2 cv::CONTOURS_MATCH_I2
#define COMPAT_CV_FILLED cv::FILLED
#define COMPAT_CV_LINE_AA cv::LINE_AA
#define compat_cv_rotate cv::rotate
#endif

#endif /* COMPAT_OPENCV_H */
