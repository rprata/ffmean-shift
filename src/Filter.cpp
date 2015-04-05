#include "Filter.h"

void Filter::StartFilter(AVFrame * pFrame, int width, int height, void (Filter::*f)(AVFrame *, int, int))
{
	int  y, k;
	for(y = 1; y < height -1; y++)
		for (k = 3; k < 3 * (width - 1); k += 3)
		{
			(this->*f)(pFrame, y, k);
		}
}

void Filter::GrayFilter(AVFrame * pFrame, int y, int k)
{
	float r, g, b;
	uint8_t out;
	uint8_t * bufferRGB = pFrame->data[0] + y*pFrame->linesize[0] + k;

	r = (float)(0.114f * (int) *(bufferRGB));
	g = (float)(0.587f * (int) *(bufferRGB + 1));
	b = (float)(0.299f * (int) *(bufferRGB + 2));
	out = (uint8_t) (r + g + b);
	*(bufferRGB) = out;
	*(bufferRGB + 1) = out;
	*(bufferRGB + 2) = out;
}


void Filter::SimpleGaussianFilter(AVFrame * pFrame, int y, int k)
{
	uint8_t * center = (pFrame->data[0] + y*pFrame->linesize[0] + k);
	uint8_t * left = (pFrame->data[0] + y*pFrame->linesize[0] + k - 3);
	uint8_t * right = (pFrame->data[0] + y*pFrame->linesize[0] + k + 3);
	uint8_t * top =  (pFrame->data[0] + (y - 1)*pFrame->linesize[0] + k);
	uint8_t * bottom = (pFrame->data[0] + (y + 1)*pFrame->linesize[0] + k);

	float pixel;
	uint8_t out;
	
	pixel = (float) ((float) ((int) *(center)) + (float) ((int) *(left)) + (float) ((int) *(right)) + (float) ((int) *(top)) + (float) ((int) *(bottom)))/5.0f;
	out = (uint8_t) pixel;
	*(center) = out;

	pixel = (float) ((float) ((int) *(center + 1)) + (float) ((int) *(left + 1)) + (float) ((int) *(right + 1)) + (float) ((int) *(top + 1)) + (float) ((int) *(bottom + 1) + 1))/5.0f;
	out = (uint8_t) pixel;
	*(center + 1) = out;

	pixel = (float) ((float) ((int) *(center + 2)) + (float) ((int) *(left + 2)) + (float) ((int) *(right + 2)) + (float) ((int) *(top + 2)) + (float) ((int) *(bottom + 1) + 2))/5.0f;
	out = (uint8_t) pixel;
	*(center + 2) = out;
}

void Filter::ComplexGaussianFilter(AVFrame * pFrame, int y, int k)
{
	uint8_t * center = (pFrame->data[0] + y*pFrame->linesize[0] + k);
	uint8_t * left = (pFrame->data[0] + y*pFrame->linesize[0] + k - 3);
	uint8_t * right = (pFrame->data[0] + y*pFrame->linesize[0] + k + 3);
	uint8_t * top =  (pFrame->data[0] + (y - 1)*pFrame->linesize[0] + k);
	uint8_t * bottom = (pFrame->data[0] + (y + 1)*pFrame->linesize[0] + k);

	float pixel;
	uint8_t out;
	float k1, k2, k3, k4;

	k1 = sqrt((float)((*center - *left)*(*center - *left)));
	k2 = sqrt((float)((*center - *right)*(*center - *right)));
	k3 = sqrt((float)((*center - *top)*(*center - *top)));
	k4 = sqrt((float)((*center - *bottom)*(*center - *bottom)));


	pixel = (float) ((float) ((int) *(center)) + (float) ((int) *(left)*k1) + 
		(float) ((int) *(right)*k2) + (float) ((int) *(top)*k3) + 
		(float) ((int) *(bottom)*k4))/(float)(1 + k1 + k2 + k3 +k4);
	out = (uint8_t) pixel;
	*(center) = out;

	k1 = sqrt((float)((*(center + 1) - *(left + 1))*(*(center + 1) - *(left + 1))));
	k2 = sqrt((float)((*(center + 1) - *(right + 1))*(*(center + 1) - *(right + 1))));
	k3 = sqrt((float)((*(center + 1) - *(top + 1))*(*(center + 1) - *(top + 1))));
	k4 = sqrt((float)((*(center + 1) - *(bottom + 1))*(*(center + 1) - *(bottom + 1))));

	pixel = (float) ((float) ((int) *(center + 1)) + (float) ((int) *(left + 1)*k1) + 
		(float) ((int) *(right + 1)*k2) + (float) ((int) *(top + 1)*k3) + 
		(float) ((int) *(bottom + 1)*k4 + 1))/(float)(1 + k1 + k2 + k3 +k4);
	out = (uint8_t) pixel;
	*(center + 1) = out;

	k1 = sqrt((float)((*(center + 2) - *(left + 2))*(*(center + 2) - *(left + 1))));
	k2 = sqrt((float)((*(center + 2) - *(right + 2))*(*(center + 2) - *(right + 1))));
	k3 = sqrt((float)((*(center + 2) - *(top + 2))*(*(center + 2) - *(top + 1))));
	k4 = sqrt((float)((*(center + 2) - *(bottom + 2))*(*(center + 2) - *(bottom + 1))));

	pixel = (float) ((float) ((int) *(center + 2)) + (float) ((int) *(left + 2)*k1) + 
		(float) ((int) *(right + 2)*k2) + (float) ((int) *(top + 2)*k3) + 
		(float) ((int) *(bottom + 2)*k4 + 1))/(float)(1 + k1 + k2 + k3 +k4);	
	out = (uint8_t) pixel;
	*(center + 2) = out;
}
