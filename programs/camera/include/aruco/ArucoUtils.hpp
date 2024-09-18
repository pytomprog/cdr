#pragma once

#ifndef CDR_ARUCOUTILS_HPP
#define CDR_ARUCOUTILS_HPP

#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>


void improveFutureCropRectangle(cv::Mat& inputFrame, std::vector<cv::Point2f>& markerCorners, cv::Rect& futureCropRectangle);
float markerLength(int markerId);
cv::Mat marker3dPoints(int markerId);
cv::Mat tableMarkers3dPoints(std::vector<int> markersId);
cv::Mat table3dPoints();
void inversePerspective(cv::Vec3f& rvec, cv::Vec3f& tvec, cv::Vec3f& invRvec, cv::Vec3f& invTvec);

#endif // CDR_ARUCOUTILS_HPP
