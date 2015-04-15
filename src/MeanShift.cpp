#include "MeanShift.h"

void MeanShift::SetupMeanShift()
{
	for (int i = 0; i < NBINS; i++) 
	{
		q_u.R[i] = 0;
		q_u.G[i] = 0;
		q_u.B[i] = 0;

		p_u.R[i] = 0;
		p_u.G[i] = 0;
		p_u.B[i] = 0;
	}
}

MeanShift::Histogram MeanShift::CalculateHistogram(AVFrame * pFrame, int imageW, int imageH, int x, int y, int width, int height) 
{	
	Histogram histogram;
	
	for (int p = 0; p < NBINS; p++) 
	{
		histogram.R[p] = 0;
		histogram.G[p] = 0;
		histogram.B[p] = 0;
	}

	uint8_t * bufferRGB = NULL;
	uint8_t r, g, b;
	float total = 0, normalDistribution;
	int cx = x + width / 2;	
	int cy = y + height / 2;

	for (int i = 0; i < imageH; i++) 
	{
		for (int k = 0; k < 3 * imageW; k += 3)
		{
			bufferRGB = pFrame->data[0] + i*pFrame->linesize[0] + k;
			if ((k/3 >= x)  && (k/3 <= x + width) && (i >= y) && (i <= y + height))
			{
				r = *(bufferRGB);
				g = *(bufferRGB + 1);
				b = *(bufferRGB + 2);
				
				normalDistribution = exp(-0.5 * ((cx - i) * (cx - i) + (cy - k / 3) * (cy - k / 3))/(width * width / 16));

				histogram.R[r/NBINS] += normalDistribution;
				histogram.G[g/NBINS] += normalDistribution;
				histogram.B[b/NBINS] += normalDistribution;
				total += 3 * normalDistribution;
			}
		}
	}
 	
 	for (int j = 0; j < NBINS; j++)
 	{
 		histogram.R[j] = histogram.R[j]/total;
		histogram.G[j] = histogram.G[j]/total;
		histogram.B[j] = histogram.B[j]/total;
 	}

	return histogram;
}

void MeanShift::SetupQVector(AVFrame * pFrame, int imageW, int imageH, int x, int y, int width, int height)
{
	q_u = CalculateHistogram(pFrame, imageW, imageH, x, y, width, height);
	m_imageW = imageW;
	m_imageH = imageH;
	m_x = x;
	m_y = y;
	m_width = width;
	m_height = height;
}

void MeanShift::SetupPVector(AVFrame * pFrame)
{
	p_u = CalculateHistogram(pFrame, m_imageW, m_imageH, m_x, m_y, m_width, m_height);
}

void MeanShift::StartMeanShift(AVFrame * pFrame)
{
	p_y0_q = 0;

	for (int i = 0; i < 16; i++) 
	{
		p_y0_q += sqrt(q_u.R[i]*p_u.R[i]);
		p_y0_q += sqrt(q_u.G[i]*p_u.G[i]);
		p_y0_q += sqrt(q_u.B[i]*p_u.B[i]);
	}
	
	//TODO: rever essa eq.	
	// Histogram histogram;
	// for (int i = m_x - m_width/4; i < m_width; i++)
	// {
	// 	if (i < 0) continue;
	// 	for (int j = m_y - m_height/4; j < m_height; j++)
	// 	{
	// 		if (j < 0) continue;
	// 		// histogram = CalculateHistogram(pFrame, m_imageW, m_imageH, i, j, m_width, m_height);
	// 		for (int k = 0; k < NBINS; k++)
	// 		{

	// 		}
			
	// 	}
	// }
	weight = 0;
	for (int k = 0; k < NBINS; k++)
	{
		weight += sqrt(q_u.R[k]/p_u.R[k]);
		weight += sqrt(q_u.G[k]/p_u.G[k]);
		weight += sqrt(q_u.B[k]/p_u.B[k]);
	}
	
	int cx = x + width / 2;	
	int cy = y + height / 2;
	
	// p_y1_q = exp(-0.5 * ((cx - i) * (cx - i) + (cy - k / 3) * (cy - k / 3))/(width * width / 16));
}
