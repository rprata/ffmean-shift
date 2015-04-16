#ifndef MEANSHIFT_H
#define MEANSHIFT_H

#include <cmath>
#include <unistd.h>

extern "C" {
#include <libavformat/avformat.h>
}

#define NBINS 16

class MeanShift {

typedef struct
{
	double R[NBINS];
	double G[NBINS];
	double B[NBINS];
} Histogram;

private:
	Histogram q_u, p_u;

	double p_y0_q;
	double p_y1_q;
	double y0[2];
	double y1[2];

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
	double * getVectorY();

};

#endif