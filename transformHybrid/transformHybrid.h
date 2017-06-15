#pragma once
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// typedef struct VideoFrameTransform VideoFrameTransform;

extern bool transformVideoFrame(
// extern bool VideoFrameTransform_transformVideoFrame(
      // VideoFrameTransform* transform,
      uint8_t* inputData,
      uint8_t* outputData,
	int inputWidth,
	int inputHeight,
	int inputWidthWithPadding,
	int outputWidth,
	int outputHeight,
	int outputWidthWithPadding);

#ifdef __cplusplus
}
#endif
