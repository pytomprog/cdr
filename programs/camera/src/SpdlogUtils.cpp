#include "SpdlogUtils.hpp"

auto fmt::formatter<cv::Mat>::format(const cv::Mat& mat, format_context& ctx) const -> decltype(ctx.out()) {
	auto out = ctx.out();
	fmt::format_to(out, "[");
	for (int i = 0; i < mat.rows; ++i) {
		for (int j = 0; j < mat.cols; ++j) {
			if (mat.channels() == 1) {
				fmt::format_to(out, "{}", mat.at<float>(i, j));
			} else if (mat.channels() == 3) {
				cv::Vec3f pixel = mat.at<cv::Vec3f>(i, j);
				fmt::format_to(out, "[{}, {}, {}]", pixel[0], pixel[1], pixel[2]);
			}
			if (j < mat.cols - 1) {
				fmt::format_to(out, ", ");
			}
		}
		if (i < mat.rows - 1) {
			fmt::format_to(out, ";\n ");
		} else {
			fmt::format_to(out, "]");
		}
	}
	return out;
}

auto fmt::formatter<cv::Vec3f>::format(const cv::Vec3f& vec, format_context& ctx) const -> decltype(ctx.out()) {
	auto out = ctx.out();
	fmt::format_to(out, "[{}, {}, {}]", vec[0], vec[1], vec[2]);
	return out;
}