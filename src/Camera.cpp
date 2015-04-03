#include "includes/Camera.h"

bool Camera::SetupCamera(std::string m_device)
{
	pFormatCtx = NULL;

	sws_ctx = NULL;
	out_sws_ctx = NULL;
	c = NULL;

	av_register_all();
	avdevice_register_all();

	AVInputFormat *iformat = av_find_input_format("video4linux2");

	if (!iformat)
	{
		fprintf(stderr, "Nao foi possivel abrir o dispostivo\n");
		return false;	
	}

	//Abre o arquivo de midia;	
	if (avformat_open_input(&pFormatCtx, m_device.c_str(), iformat, NULL) != 0)
	{
		fprintf(stderr, "Nao foi possivel abrir o arquivo %s\n", m_device.c_str());
		return false;
	}

	//Recupera a informacao do stream;
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
  	{
  		fprintf(stderr, "Nao foi possivel encontrar a informacao do stream\n");
  		return false; 
  	}

  	//Informacao bruta sobre o arquivo de video;
	av_dump_format(pFormatCtx, 0, m_device.c_str(), 0);

	//Encontra o primeiro stream de video (video principal)
	m_videostream = -1;
	
	for (unsigned i = 0; i < pFormatCtx->nb_streams; i++)
	{
		if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			m_videostream = i;
			break;
		}
	}

	if (m_videostream == -1)
	{
		fprintf(stderr, "Nao foi possivel encontrar o stream de video\n");
		return false;
	}

	//Captura o ponteiro referente ao codec do stream de video
	pCodecCtx = pFormatCtx->streams[m_videostream]->codec;

	//Busca o decode do video
	if ((pCodec = avcodec_find_decoder(pCodecCtx->codec_id)) == NULL)
	{
		fprintf(stderr, "Codec nao suportado :(\n");
		return false;
	}

	//Abre o codec	
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		fprintf(stderr, "Nao foi possivel abrir o codec\n");
		return false;
	}


	// Aloca espaco de memoria para o frame de video (AVFrame)
	pDecodedFrame = avcodec_alloc_frame();

	if ((pFrameRGB = avcodec_alloc_frame()) == NULL)
  	{
  		fprintf(stderr, "Nao foi possivel alocar memoria para o frame de video\n");
	  	return false;
  	}

  	if ((pOutputFrame = avcodec_alloc_frame()) == NULL)
  	{
  		fprintf(stderr, "Nao foi possivel alocar memoria para o frame de video\n");
	  	return false;
  	}
	
	//Determina o tamanho necessario do buffer e aloca a memoria
	numBytesRGB = avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
	
	bufferRGB = (uint8_t *) av_malloc(numBytesRGB*sizeof(uint8_t));

	//Configura o contexto para o escalonamento
	sws_ctx = sws_getContext (
	        pCodecCtx->width,
	        pCodecCtx->height,
	        pCodecCtx->pix_fmt,
	        pCodecCtx->width,
	        pCodecCtx->height,
	        PIX_FMT_RGB24,
	        SWS_BILINEAR,
	        NULL,
	        NULL,
	        NULL
	);

	//Aplica para o buffer os frames no formato FMT_RGB24 (pacote RGB 8:8:8, 24bpp, RGBRGB...)
	avpicture_fill((AVPicture *) pFrameRGB, bufferRGB , PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);

	//Preparando AVCodecContext de saida
	codec = avcodec_find_encoder(pFormatCtx->streams[m_videostream]->codec->codec_id);

	if (!codec)
	{
		fprintf(stderr, "Codec nao encontrado\n");
		return false;
	}

	c = avcodec_alloc_context3(codec);

	//Configurando valores para o contexto do video de saida
    c->bit_rate = pCodecCtx->bit_rate;
    c->width = pCodecCtx->width;
    c->height = pCodecCtx->height;
    c->time_base = pCodecCtx->time_base;
    c->gop_size = pCodecCtx->gop_size;
    c->max_b_frames = pCodecCtx->max_b_frames;
    c->pix_fmt = PIX_FMT_YUV420P;

    if (avcodec_open2(c, codec, NULL) < 0) 
    	return false;

    //Criacao de contexto para converter um tipo RGB24 para YUV240P (preparacao para encoded)
    numBytesYUV = avpicture_get_size(PIX_FMT_YUV420P, c->width, c->height);
	
	bufferYUV = (uint8_t *) av_malloc(numBytesYUV*sizeof(uint8_t));

    out_sws_ctx = sws_getContext (
	        c->width,
	        c->height,
	        PIX_FMT_RGB24,
	       	c->width,
	        c->height,
	        PIX_FMT_YUV420P,
	        SWS_FAST_BILINEAR,
	        NULL,
	        NULL,
	        NULL
	);

	avpicture_fill((AVPicture *) pOutputFrame, bufferYUV , PIX_FMT_YUV420P, c->width, c->height);
	outbuf_size = 300000;
	outbuf = NULL;
	outbuf = (uint8_t *) av_malloc(outbuf_size);

	avpicture_alloc(&pLastBufferRGB, PIX_FMT_RGB24, c->width, c->height);

	if ((bmp = SetupSDL(pCodecCtx, bmp)) == NULL) 
	{
		return false;
	}

	return true;
}


bool Camera::StartCamera()
{
	while (true) 
	{
		av_read_frame(pFormatCtx, &m_packet);
	  	//Testa se e unm pacote com de stream de video
	  	if(m_packet.stream_index == m_videostream) 
	  	{
			// Decode frame de video
		    avcodec_decode_video2(pCodecCtx, pDecodedFrame, &m_frameFinished, &m_packet);

		    //Testa se ja existe um quadro de video
		    if (m_frameFinished) 
		    {
				
				SDL_LockYUVOverlay(bmp);
		   		
		   		//Converte a imagem de seu formato nativo para RGB
		   		sws_scale
				(
					sws_ctx,
					(uint8_t const * const *) pDecodedFrame->data,
					pDecodedFrame->linesize,
					0,
					pCodecCtx->height,
					pFrameRGB->data,
					pFrameRGB->linesize
				);
				
				Filter(pFrameRGB, pCodecCtx->width, pCodecCtx->height, &Camera::SimpleGaussianFilter);

				//Convertendo de RGB para YUV
				sws_scale (
					out_sws_ctx, 
					(uint8_t const * const *) pFrameRGB->data, 
					pFrameRGB->linesize, 
        			0, 
        			c->height, 
        			pOutputFrame->data, 
        			pOutputFrame->linesize
        		);	

        		pOutputFrame->data[0] = bmp->pixels[0];
				pOutputFrame->data[1] = bmp->pixels[2];
				pOutputFrame->data[2] = bmp->pixels[1];

				pOutputFrame->linesize[0] = bmp->pitches[0];
				pOutputFrame->linesize[1] = bmp->pitches[2];
				pOutputFrame->linesize[2] = bmp->pitches[1];

				SDL_UnlockYUVOverlay(bmp);

				rect.x = 0;
				rect.y = 0;
				rect.w = c->width;
				rect.h = c->height;

				SDL_DisplayYUVOverlay(bmp, &rect);
		     }	
    	}
		av_free_packet(&m_packet);

		SDL_PollEvent(&event);
	    
	    switch(event.type) 
	    {
	    	case SDL_QUIT: SDL_Quit();
	    		return 0;
	      		break;
	    	default:
	      		break;
	    }
    }
  
	return true;
}


SDL_Overlay * Camera::SetupSDL(AVCodecContext * pCodecCtx, SDL_Overlay * bmp) 
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) 
	{
    	fprintf(stderr, "Nao foi possivel inicializar o SDL - %s\n", SDL_GetError());
    	return NULL;
  	}

  	SDL_Surface * screen;

	screen = SDL_SetVideoMode(pCodecCtx->width, pCodecCtx->height, 0, 0);
	if (!screen) 
	{
  		fprintf(stderr, "SDL: Nao foi possivel configurar o modo do video\n");
  		return NULL;
	}

	bmp = SDL_CreateYUVOverlay(pCodecCtx->width, pCodecCtx->height, SDL_YV12_OVERLAY, screen);
	
  	return bmp;
}

void Camera::Filter(AVFrame * pFrame, int width, int height, void (Camera::*f)(AVFrame *, int, int))
{
	int  y, k;
	for(y = 1; y < height -1; y++)
		for (k = 3; k < 3 * (width - 1); k += 3)
		{
			(this->*f)(pFrame, y, k);
		}
}

void Camera::GrayFilter(AVFrame * pFrame, int y, int k)
{
	float r, g, b;
	uint8_t out;
	uint8_t * bufferRGB = pFrame->data[0] + y*pFrame->linesize[0] + k;

	b = (float)(0.114f * (int) *(bufferRGB));
	g = (float)(0.587f * (int) *(bufferRGB + 1));
	r = (float)(0.299f * (int) *(bufferRGB + 2));
	out = (uint8_t) (r + g + b);
	*(bufferRGB) = out;
	*(bufferRGB + 1) = out;
	*(bufferRGB + 2) = out;
}


void Camera::SimpleGaussianFilter(AVFrame * pFrame, int y, int k)
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

void Camera::ComplexGaussianFilter(AVFrame * pFrame, int y, int k)
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
