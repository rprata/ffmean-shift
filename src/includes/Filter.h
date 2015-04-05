#ifndef FILTER_H
#define FILTER_H

extern "C" {
#include <libavformat/avformat.h>
}

class Filter {

public:
	void GrayFilter(AVFrame * pFrame, int y, int k);
	void SimpleGaussianFilter(AVFrame * pFrame, int y, int k);
	void ComplexGaussianFilter(AVFrame * pFrame, int y, int k);

	void StartFilter(AVFrame * pFrame, int width, int height, void (Filter::*f)(AVFrame *, int, int));
	
};

#endif