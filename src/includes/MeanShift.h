#ifndef MEANSHIFT_H
#define MEANSHIFT_H

extern "C" {
#include <libavformat/avformat.h>
}

class MeanShift {

typedef struct
{
	float R[16];
	float G[16];
	float B[16];
} Histogram;

private:
	Histogram histogram;
public:
	void CalculateHistogram(AVFrame * pFrame, int imageW, int imageH, int x, int y, int width, int height);
	void SetupMeanShift();

};

#endif