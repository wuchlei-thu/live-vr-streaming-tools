#pragma once
#include <opencv2/opencv.hpp>

bool createCapFace(const cv::Mat &in, cv::Mat &face,
	int faceId = 0, const int width = -1,
	const int height = -1, const float degree = M_PI/6);

bool transform(
	const cv::Mat &image, 
	cv::Mat &output,
	int outputWidth,
	int outputHeight, 
	const float degree = M_PI / 6);

bool tranformToHybridLayout(
	uint8_t* inputArray,
	uint8_t* outputArray,
	int inputWidth,
	int inputHeight,
	int inputWidthWithPadding,
	int outputWidth,
	int outputHeight,
	int outputWidthWithPadding,
	const float degree = M_PI / 6);