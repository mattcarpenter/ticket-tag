#pragma once
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include <opencv2/core.hpp>

class TicketTag
{
public:
	TicketTag(std::string ticket_number);
	TicketTag(cv::Mat image);
	~TicketTag();
	static std::vector<TicketTag> TicketTag::extract_from_image(std::string path);
	cv::Mat get_image();
private:
	cv::Mat image;
};
