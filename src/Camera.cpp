#include "Camera.h"

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
	Draw d;
	Uint32 start;

	bool mouseButtonSecond = false;
	int counterClicks = 1;

	int m_x = 0;
	int m_y = 0;
	int m_width = 0;
	int m_height = 0;

	MeanShift meanshift;
	meanshift.SetupMeanShift();

	while (true) 
	{
		av_read_frame(pFormatCtx, &m_packet);
	  	//Testa se e unm pacote com de stream de video
	  	if(m_packet.stream_index == m_videostream) 
	  	{
	  		start = SDL_GetTicks();
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
				
				SDL_PollEvent(&event);
		
				switch(event.type) 
				{
					case SDL_QUIT: 
						SDL_Quit();
						return true;
				  	case SDL_MOUSEBUTTONDOWN:
				  		if (counterClicks % 2 == 0 && counterClicks > 0)
				  		{
				  			mouseButtonSecond = true;
				  			m_width = event.button.x;
				  			m_height = event.button.y;
				  			counterClicks = 1;
				  		}
				  		else 
				  		{	
				  			mouseButtonSecond = false;
				  			m_x = event.button.x;
				  			m_y = event.button.y;
				  			counterClicks++;
				  		}
				  		break;
					default:
				  		break;
				}

				if (mouseButtonSecond)
				{
					int thickness = 1;
					d.DrawRect (pFrameRGB, pCodecCtx->width, pCodecCtx->height, m_x, m_y, m_width, m_height, 0xF00D42, thickness);
					meanshift.CalculateHistogram(pFrameRGB, pCodecCtx->width, pCodecCtx->height, m_x + thickness, m_y + thickness, m_width - thickness, m_height - thickness);
				}

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

				if(1000 / FPS > SDL_GetTicks() - start) 
				{
                	SDL_Delay(1000 / FPS - (SDL_GetTicks() - start));
                }

		     }	
    	}
		av_free_packet(&m_packet);

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
  	
  	SDL_WM_SetCaption("FFMean-Shift", NULL);

	SDL_Surface * screen = SDL_SetVideoMode(pCodecCtx->width, pCodecCtx->height, 32, SDL_SWSURFACE);
	if (!screen) 
	{
  		fprintf(stderr, "SDL: Nao foi possivel configurar o modo do video\n");
  		return NULL;
	}

	bmp = SDL_CreateYUVOverlay(pCodecCtx->width, pCodecCtx->height, SDL_YV12_OVERLAY, screen);
	
  	return bmp;
}