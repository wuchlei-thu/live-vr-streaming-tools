#include "transformHybrid.h"
#include "transformHybrid_helper.h"

bool transformVideoFrame(
// bool VideoFrameTransform_transformVideoFrame(
      // VideoFrameTransform* transform,
      uint8_t* inputData,
      uint8_t* outputData,
	int inputWidth,
	int inputHeight,
	int inputWidthWithPadding,
	int outputWidth,
	int outputHeight,
	int outputWidthWithPadding){

      // return tranformToHybridLayout(inputData, outputData, M_PI / 6);

      return tranformToHybridLayout(
      inputData,
      outputData,
      inputWidth,
	inputHeight,
	inputWidthWithPadding,
	outputWidth,
	outputHeight,
	outputWidthWithPadding,
	M_PI / 6);
}
