#pragma once

#ifndef CDR_ARUCOUTILS_HPP
#define CDR_ARUCOUTILS_HPP

#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

void improveFutureCropRectangle(cv::Mat& inputFrame, std::vector<cv::Point2f>& markerCorners, cv::Rect& futureCropRectangle);

#endif // CDR_ARUCOUTILS_HPP