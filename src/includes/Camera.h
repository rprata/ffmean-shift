#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <cstdio>
#include <cstdlib>

extern "C" {

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <libavutil/pixfmt.h>
#include <libavutil/mathematics.h>

#include <SDL.h>
#include <SDL_thread.h>
}

class Camera
{

private:
	AVFormatContext * pFormatCtx;
	AVCodecContext * pCodecCtx;
	AVCodecContext * c;
	AVCodec * pCodec;
	AVCodec * codec;
	AVFrame * pFrameRGB, * pDecodedFrame, * pOutputFrame;
	AVPacket m_packet;
	AVPicture pLastBufferRGB;
	
	struct SwsContext * sws_ctx;
	struct SwsContext * out_sws_ctx;

	uint8_t * outbuf;
	uint8_t * bufferRGB;
	uint8_t * bufferYUV;
	
	int outbuf_size;
	int out_size;
	int m_videostream;
	int m_frameFinished;
	int numBytesRGB;
	int numBytesYUV;

	SDL_Overlay * bmp;
	SDL_Rect rect;
	SDL_Event event;

	SDL_Overlay * SetupSDL(AVCodecContext * pCodecCtx, SDL_Overlay * bmp);
	void Filter(AVFrame * pFrame, int width, int height, void (Camera::*f)(AVFrame *, int, int));
	void GrayFilter(AVFrame * pFrame, int y, int k);
	void SimpleGaussianFilter(AVFrame * pFrame, int y, int k);
	void ComplexGaussianFilter(AVFrame * pFrame, int y, int k);

public:
	bool SetupCamera(std::string m_device);
	bool StartCamera();

};

#endif
