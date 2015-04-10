#include "MeanShift.h"

void MeanShift::SetupMeanShift()
{
	for (int i = 0; i < 16; i++) 
	{
		histogram.R[i] = 0;
		histogram.G[i] = 0;
		histogram.B[i] = 0;
	}
}

void MeanShift::CalculateHistogram(AVFrame * pFrame, int imageW, int imageH, int x, int y, int width, int height) 
{	
	uint8_t * bufferRGB = NULL;
	uint8_t r, g, b;
	float total = 0;;

	for (int i = 0; i < imageH; i++) 
	{
		for (int k = 0; k < 3 * imageW; k += 3)
		{
			bufferRGB = pFrame->data[0] + i*pFrame->linesize[0] + k;
			if ((k/3 >= x)  && (k/3 <= width) && (i >= y) && (i <= height))
			{
				r = *(bufferRGB);
				g = *(bufferRGB + 1);
				b = *(bufferRGB + 2);

				int cx = (x + width)/2;
				int cy = (y + height)/2;

				//rever
				float W = exp(-0.5 * ((cx-i)*(cx-i) + (cy - k / 3)*(cy - k / 3))/(width*width / 16));

				histogram.R[r/16] += W;
				histogram.G[g/16] += W;
				histogram.B[b/16] += W;
				total += 3 * W;

			}
		}
	}

	printf("******************* Histogram **********************\n");
	for (int i = 0; i < 16; i++) 
	{
		printf("R %i - %f \t", i, histogram.R[i]/total);
		printf("G %i - %f \t", i, histogram.G[i]/total);
		printf("B %i - %f \n", i, histogram.B[i]/total);
	}

	for (int i = 0; i < 16; i++) 
	{
		histogram.R[i] = 0;
		histogram.G[i] = 0;
		histogram.B[i] = 0;
	}
}