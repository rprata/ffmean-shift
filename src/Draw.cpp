#include "Draw.h"

void Draw::DrawRect (AVFrame * pFrame, int imageW, int imageH,int x, int y, int width, int height, unsigned long color, int thickness)
{
	uint8_t * bufferRGB = NULL;

	for (int i = 0; i < imageH; i++) 
	{
		for (int k = 0; k < 3 * imageW; k += 3)
		{
			bufferRGB = pFrame->data[0] + i*pFrame->linesize[0] + k;
		
			if ((k/3 >= x - thickness)  && (k/3 <= x + width + thickness) && (i >= y - thickness) && (i <= y + height + thickness))
			{
				if ((k/3 <= x + thickness) || (k/3 >= x + width - thickness) ||
					(i <= y + thickness) || (i >= y + height - thickness))
				{
					*(bufferRGB) = (0xFF0000 & color) >> 16;
					*(bufferRGB + 1) = (0x00FF00 & color) >> 8;
					*(bufferRGB + 2) = (0x0000FF & color);	
				}
			}
		}
	}
}