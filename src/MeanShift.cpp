#include "MeanShift.h"

void MeanShift::SetupMeanShift()
{
	for (int i = 0; i < 256; i++) 
	{
		histogram.R[i] = 0;
		histogram.G[i] = 0;
		histogram.B[i] = 0;
	}
}

void MeanShift::CalculateHistogram(AVFrame * pFrame, int imageW, int imageH, int x, int y, int width, int height) 
{	
	uint8_t * bufferRGB = NULL;
	
	for (int i = 0; i < imageH; i++) 
	{
		for (int k = 0; k < 3 * imageW; k += 3)
		{
			bufferRGB = pFrame->data[0] + i*pFrame->linesize[0] + k;
			if ((k/3 >= x)  && (k/3 <= width) && (i >= y) && (i <= height))
			{
				histogram.R[*(bufferRGB)]++;
				histogram.G[*(bufferRGB) + 1]++;
				histogram.B[*(bufferRGB) + 2]++;
			}
		}
	}
	printf("******************* Histogram **********************\n");
	for (int i = 0; i < 256; i++) 
	{
		printf("R %x - %d \t", i, histogram.R[i]);
		printf("G %x - %d \t", i, histogram.G[i]);
		printf("B %x - %d\n", i, histogram.B[i]);
	}

	for (int i = 0; i < 256; i++) 
	{
		histogram.R[i] = 0;
		histogram.G[i] = 0;
		histogram.B[i] = 0;
	}
}