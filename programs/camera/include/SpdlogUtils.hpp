#pragma once

#ifndef CDR_SPDLOGUTILS_HPP
#define CDR_SPDLOGUTILS_HPP

#include <spdlog/spdlog.h>

#include <opencv2/core.hpp>

template <>
struct fmt::formatter<cv::Mat> : fmt::formatter<std::string> {
	auto format(const cv::Mat& mat, format_context& ctx) const -> decltype(ctx.out());
};

template <>
struct fmt::formatter<cv::Vec2f> : fmt::formatter<std::string> {
	auto format(const cv::Vec2f& vec, format_context& ctx) const -> decltype(ctx.out());
};

template <>
struct fmt::formatter<cv::Vec3f> : fmt::formatter<std::string> {
	auto format(const cv::Vec3f& vec, format_context& ctx) const -> decltype(ctx.out());
};

#endif // CDR_SPDLOGUTILS_HPP
