#include <algorithm>
#include <assert.h>
#include <array>
#include <cmath>
#include <math.h>
#include <memory>
#include <set>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "transformHybrid_helper.h"

using namespace cv;
using namespace std;

bool createCapFace(
	const Mat &in, Mat &face,
	int faceId, 
	const int width,
	const int height, 
	const float degree) {

	float inWidth = in.cols;
	float inHeight = in.rows;

	Mat mapx(height, width, CV_32F);
	Mat mapy(height, width, CV_32F);

	// Calculate adjacent (ak) and opposite (an) of the
	// triangle that is spanned from the sphere center 
	//to our cube face.
	const float an = sin(degree);
	const float ak = cos(degree);

	// For each point in the target image, 
	// calculate the corresponding source coordinates. 
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			// Map face pixel coordinates to [-1, 1] on plane
			float nx = (float)y / (float)height - 0.5f;
			float ny = (float)x / (float)width - 0.5f;

			nx *= 2;
			ny *= 2;

			// Map [-1, 1] plane coords to [-an, an]
			// thats the coordinates in respect to a unit sphere 
			// that contains our box. 
			nx *= an;
			ny *= an;

			double u, v;

			// Project from plane to sphere surface.
			if (faceId == 0) {
				// Bottom face 
				float d = sqrt(nx * nx + ny * ny);
				v = M_PI / 2 - atan2(d, ak);
				u = atan2(ny, nx);
			}
			else if(faceId == 1) {
				// Top face
				float d = sqrt(nx * nx + ny * ny);
				v = -M_PI / 2 + atan2(d, ak);
				u = atan2(-ny, nx);
			}

			// Map from angular coordinates to [-1, 1], respectively.
			u = u / (M_PI);
			v = v / (M_PI / 2);

			// Warp around, if our coordinates are out of bounds. 
			while (v < -1) {
				v += 2;
				u += 1;
			}
			while (v > 1) {
				v -= 2;
				u += 1;
			}

			while (u < -1) {
				u += 2;
			}
			while (u > 1) {
				u -= 2;
			}

			// Map from [-1, 1] to in texture space
			u = u / 2.0f + 0.5f;
			v = v / 2.0f + 0.5f;

			u = u * (inWidth - 1);
			v = v * (inHeight - 1);

			// Save the result for this pixel in map
			mapx.at<float>(x, y) = u;
			mapy.at<float>(x, y) = v;
		}
	}

	// Recreate output image if it has wrong size or type. 
	if (face.cols != width || face.rows != height ||
		face.type() != in.type()) {
		face = Mat(width, height, in.type());
	}

	// Do actual resampling using OpenCV's remap
	remap(in, face, mapx, mapy,
		CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));

	return true;
}

bool transform(
	const Mat &image, 
	Mat &output,
	int outputWidth,
	int outputHeight, 
	const float degree) {

	// output = Mat(outputHeight, outputWidth, CV_8U);
	int unit_len = outputHeight/2;

	Range r;
	r.start = image.size().height / 6;
	r.end = image.size().height / 6 + outputHeight;
	Mat middleBody = Mat(image, r, Range::all());
	//imshow("test", middleBody);
	//waitKey();
	Mat roi = output(Rect(0, 0, outputWidth-outputHeight/2, outputHeight));
	middleBody.copyTo(roi);

	for (int i = 0; i < 2; ++i) {
		Mat frame(unit_len, unit_len, CV_8UC3);
		if (!createCapFace(image, frame, i, unit_len, unit_len, M_PI / 6)) {
			//if (!createCubeMapFace(image, frame, i, unit_len, unit_len)) {
			cout << "Failed to transform" << endl;
			return -1;
		}
		else {
			//imshow("test", frame);
			//waitKey();
			Mat roi = output(Rect(outputWidth-outputHeight/2, i*unit_len, unit_len, unit_len));
			frame.copyTo(roi);
		}
	}
	return true;
}

bool tranformToHybridLayout(
	uint8_t* inputArray,
	uint8_t* outputArray,
	int inputWidth,
	int inputHeight,
	int inputWidthWithPadding,
	int outputWidth,
	int outputHeight,
	int outputWidthWithPadding,
	const float degree) {
	try {
		Mat inputMat(
		inputHeight,
		inputWidth,
		CV_8U,
		inputArray,
		inputWidthWithPadding);

		Mat outputMat(
		outputHeight,
		outputWidth,
		CV_8U,
		outputArray,
		outputWidthWithPadding);

		if (inputMat.empty())                      // Check for invalid input
		{
			cout << "Could not open or find the image" << std::endl;
			return -1;
		} else {
			imwrite("input.jpg", inputMat);
			// cvSaveImage("output.jpg" , inputMat);
		}

		// printf("Convert from array to Mat sucessfully");

		if( transform(
		inputMat,
		outputMat,
		outputWidth,
		outputHeight,
		degree) ){
			imwrite("output.jpg", outputMat);
		} else{
			cout << "Could not transform the image" << std::endl;
			return -1;
		}

		// printf("Transform from to hybrid sucessfully");
	} catch (const exception& ex) {
		printf("Could not transform the image. Error: %s\n", ex.what());
		return false;
	}
	
}

// Mat output;
// tranformToHybridLayout(image, output, M_PI / 6)