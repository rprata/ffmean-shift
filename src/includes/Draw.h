#ifndef Draw_H
#define Draw_H

extern "C" {
#include <libavformat/avformat.h>
}

class Draw {

private:

public:
	void DrawRect (AVFrame * pFrame, int imageW, int imageH,int x, int y, int width, int height, unsigned long color, int thickness);
};

#endif