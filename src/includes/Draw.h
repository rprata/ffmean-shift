#ifndef DRAW_H
#define DRAW_H

extern "C" {
#include <libavformat/avformat.h>
}

class Draw {

private:

public:
	void DrawRect (AVFrame * pFrame, int imageW, int imageH,int x, int y, int width, int height, unsigned long color, int thickness);
};

#endif