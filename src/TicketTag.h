#pragma once
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include <opencv2/core.hpp>

class TicketTag
{
public:
	TicketTag(cv::Mat image, cv::RotatedRect rRect);
	TicketTag(std::string text, cv::Mat character_map);
	~TicketTag();
	static std::vector<TicketTag> TicketTag::extract_from_image(cv::Mat src);
	cv::Mat get_image();
	cv::RotatedRect get_rect();
private:
	cv::Mat image;
	cv::RotatedRect rect;
};
