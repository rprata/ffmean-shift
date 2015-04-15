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
				
				normalDistribution = exp(-0.5 * ((cy - i) * (cy - i) + (cx - k / 3) * (cx - k / 3))/(width * width / 16));

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
	y0[0] = x + width / 2;	
	y0[1] = y + height / 2;
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
	
	//Calculo dos pesos
	double weight;
	uint8_t * bufferRGB = NULL;
	uint8_t r, g, b;

	y1[0] = y1[1] = 0;
	double delta = 0;

	for (int i = m_y; i <= m_y + m_height; i++)
	{
		for (int j = 3*(m_x); j <= 3*(m_x + m_width); j+=3)
		{
			bufferRGB = pFrame->data[0] + i*pFrame->linesize[0] + j;
			r = *(bufferRGB);
			g = *(bufferRGB + 1);
			b = *(bufferRGB + 2);
			weight = 0;
			weight += sqrt(q_u.R[r/NBINS]/p_u.R[r/NBINS]);
			weight += sqrt(q_u.G[g/NBINS]/p_u.G[g/NBINS]);
			weight += sqrt(q_u.B[b/NBINS]/p_u.B[b/NBINS]);
			//numerador da eq 3
			y1[1] += i*weight*exp(-0.5 * ((y0[1] - i) * (y0[1] - i) + (y0[0] - j / 3) * (y0[0] - j / 3))/(m_width * m_width / 16));
			y1[0] += (j/3)*weight*exp(-0.5 * ((y0[1] - i) * (y0[1] - i) + (y0[0] - j / 3) * (y0[0] - j / 3))/(m_width * m_width / 16));

			//denominador da eq 3
			delta += weight*exp(-0.5 * ((y0[1] - i) * (y0[1] - i) + (y0[0] - j / 3) * (y0[0] - j / 3))/(m_width * m_width / 16));

		}
	}

	y1[0] /= delta;
	y1[1] /= delta;

	// printf("MeanShift %f -- %f --- %f\n", y1[0], y1[1], delta);

	y0[0] = y1[0];
	y0[1] = y1[1];
	m_x = y0[0] - m_width/2;
	m_y = y0[1] - m_height/2;	
}

double * MeanShift::getVectorY()
{
	return y0;
}
