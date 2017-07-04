#include "TicketTag.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include <opencv2/core.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

#define THRESH 100

TicketTag::TicketTag(string ticket_number)
{
}

TicketTag::TicketTag(Mat image, RotatedRect rRect)
{
	this->image = image;
	this->rect = rRect;
}

vector<TicketTag> TicketTag::extract_from_image(Mat src)
{
	vector<TicketTag> ticket_tags;
	Mat src_gray;
	Mat canny_output;
	vector<Point> template_contour;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	RNG rng(12345);
	Mat contours_drawing;

  // Ticket tags should be a 5:1 rectangle
	template_contour = { Point(0, 0), Point(500, 0), Point(500, 100), Point(0, 100) };
	cvtColor(src, src_gray, CV_BGR2GRAY);

	Canny(src_gray, canny_output, THRESH, THRESH * 2, 3);
	findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	contours_drawing = Mat::zeros(canny_output.size(), CV_8UC3);

	// Enumerate each contour identified and match it against the known contour of a ticket tag
	for (int i = 0; i < contours.size(); i++)
	{
		double result = matchShapes(contours[i], template_contour, CV_CONTOURS_MATCH_I1, 0);

		if (result < 0.04 && result > 0) {
			// Found a contour that looks like a ticket tag. Rotate, crop, and perform OCR
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			drawContours(contours_drawing, contours, i, color, 2, 8, hierarchy, 0, Point());

			RotatedRect rRect = minAreaRect(contours[i]);
			Point2f vertices[4];
			rRect.points(vertices);

			Mat M, rotated, cropped;
			// get angle and size from the bounding box
			float angle = rRect.angle;
			Size rect_size = rRect.size;

			if (rRect.angle < -45.) {
				angle += 90.0;
				swap(rect_size.width, rect_size.height);
			}

			// get the rotation matrix
			M = getRotationMatrix2D(rRect.center, angle, 1.0);

			// perform the affine transformation
			warpAffine(src, rotated, M, src.size(), INTER_CUBIC);

			// crop the resulting image
			getRectSubPix(rotated, rect_size, rRect.center, cropped);

			ticket_tags.push_back(TicketTag(cropped, rRect));
		}
	}

	return ticket_tags;
}

Mat TicketTag::get_image()
{
  return this->image;
}

RotatedRect TicketTag::get_rect()
{
  return this->rect;
}

TicketTag::~TicketTag()
{
}
