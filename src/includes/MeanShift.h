#ifndef MEANSHIFT_H
#define MEANSHIFT_H

extern "C" {
#include <libavformat/avformat.h>
}

class MeanShift {

typedef struct
{
	uint8_t R[256];
	uint8_t G[256];
	uint8_t B[256];
} Histogram;

private:
	Histogram histogram;
public:
	void CalculateHistogram(AVFrame * pFrame, int imageW, int imageH, int x, int y, int width, int height);
	void SetupMeanShift();

};

#endif