#ifndef MEANSHIFT_H
#define MEANSHIFT_H

#include <cmath>

extern "C" {
#include <libavformat/avformat.h>
}

#define NBINS 16

class MeanShift {

typedef struct
{
	float R[NBINS];
	float G[NBINS];
	float B[NBINS];
} Histogram;

private:
	Histogram q_u, p_u;
	double weight;
	double p_y0_q, p_y1_q;

	int m_imageW;
	int m_imageH;
	int m_x;
	int m_y; 
	int m_width;
	int m_height;

public:
	MeanShift::Histogram CalculateHistogram(AVFrame * pFrame, int imageW, int imageH, int x, int y, int width, int height);
	void SetupQVector(AVFrame * pFrame, int imageW, int imageH, int x, int y, int width, int height);
	void SetupPVector(AVFrame * pFrame);
	void SetupMeanShift();
	void StartMeanShift(AVFrame * pFrame);

};

#endif