#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "implot.h"

#include <SDL.h>

#include "harakiri.hh"

int main(int argc, char** argv)
{
  if (argc < 2) {
    printf("You need to specify a media file.\n");
    return -1;
  }

  AV_Context_Sosite_Chlen hui;
  if (load_mp4(hui, argv[1]) != 0)
    return -1;

  SDL_Init(SDL_INIT_VIDEO);
  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

  SDL_Window* window = SDL_CreateWindow("how many bananas can fit?",
					SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720,
					// SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
					SDL_WINDOW_ALLOW_HIGHDPI
					);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  ImPlot::CreateContext();

  // темная тема
  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer2_Init(renderer);

  int   bar_data[11] = { 2, 2, 6, 4, 5, 6, 2, 8, 9, 10, 11 };
  float x_data[1000];
  float y_data[1000];
  for (int i = 0; i < 1000; i++) {
    float x = i*0.01;
    x_data[i] = x;
    y_data[i] = sin(x) + 0.5 * cos(2 * x) + 0.1 * sin(4*x);
  }

  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Main loop
  bool done = false;
  while (!done)
    {
      SDL_Event event;
      while (SDL_PollEvent(&event))
	{
	  ImGui_ImplSDL2_ProcessEvent(&event);
	  switch (event.type)
	    {
	    case SDL_QUIT:
	      done = true;
	      break;
	    case SDL_WINDOWEVENT:
	      if (event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
		done = true;
	      break;
	    case SDL_KEYDOWN:
	      switch (event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
		  done = true;
		  break;
		}
	      break;
	    }
	}

      ImGui_ImplSDLRenderer2_NewFrame();
      ImGui_ImplSDL2_NewFrame();
      ImGui::NewFrame();

      if (show_demo_window)
	ImGui::ShowDemoWindow(&show_demo_window);

      if (0) {
	static float f = 0.0f;
	static int counter = 0;

	ImGui::Begin("Hello, world!");

	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	ImGui::Checkbox("Another Window", &show_another_window);

	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
	  counter++;
	ImGui::SameLine();
	ImGui::Text("counter = %d", counter);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	ImGui::End();
      }

      ImGui::Begin("My Window");
      if (ImPlot::BeginPlot("My Plot")) {
	ImPlot::PlotBars("My Bar Plot", bar_data, 11);
	ImPlot::PlotLine("My Line Plot", x_data, y_data, 1000);
	ImPlot::EndPlot();
      }
      ImGui::End();

      // 3. Show another simple window.
      if (show_another_window) {
	  ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	  ImGui::Text("Hello from another window!");
	  if (ImGui::Button("Close Me"))
	    show_another_window = false;
	  ImGui::End();
	}

      ImGui::Render();
      SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
      SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
      SDL_RenderClear(renderer);
      ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
      SDL_RenderPresent(renderer);
    }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  ImPlot::DestroyContext();
  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  SDL_Quit();

#if 0
  AVFormatContext* pFormatContext = avformat_alloc_context();

  if (avformat_open_input(&pFormatContext, argv[1], NULL, NULL) != 0) {
    printf("ERROR: could not open the file %s\n", argv[1]);
    return -1;
  }

  printf("format %s, duration %lld us, bit_rate %lld\n", pFormatContext->iformat->name, pFormatContext->duration, pFormatContext->bit_rate);

  if (avformat_find_stream_info(pFormatContext,  NULL) < 0) {
    printf("ERROR could not get the stream info\n");
    return -1;
  }

  const AVCodec *pCodec = NULL;
  AVCodecParameters *pCodecParameters =  NULL;
  int video_stream_index = -1;

  printf("number of streams: %d\n", pFormatContext->nb_streams);

  for (int i = 0; i < pFormatContext->nb_streams; i++) {
    AVCodecParameters *pLocalCodecParameters =  NULL;
    pLocalCodecParameters = pFormatContext->streams[i]->codecpar;
    printf("AVStream->time_base before open coded %d/%d\n", pFormatContext->streams[i]->time_base.num, pFormatContext->streams[i]->time_base.den);
    printf("AVStream->r_frame_rate before open coded %d/%d\n", pFormatContext->streams[i]->r_frame_rate.num, pFormatContext->streams[i]->r_frame_rate.den);
    printf("AVStream->start_time %d\n", pFormatContext->streams[i]->start_time);
    printf("AVStream->duration %d\n", pFormatContext->streams[i]->duration);

    printf("finding the proper decoder (CODEC)\n");

    const AVCodec *pLocalCodec = NULL;

    // finds the registered decoder for a codec ID
    // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga19a0ca553277f019dd5b0fec6e1f9dca
    pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);

    if (pLocalCodec==NULL) {
      printf("ERROR unsupported codec!\n");
      // In this example if the codec is not found we just skip it
      continue;
    }

    // when the stream is a video we store its index, codec parameters and codec
    if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
      if (video_stream_index == -1) {
	video_stream_index = i;
	pCodec = pLocalCodec;
	pCodecParameters = pLocalCodecParameters;
      }

      printf("Video Codec: resolution %d x %d\n", pLocalCodecParameters->width, pLocalCodecParameters->height);
    } else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
      printf("Audio Codec: %d channels, sample rate %d\n", pLocalCodecParameters->channels, pLocalCodecParameters->sample_rate);
    }

    // print its name, id and bitrate
    printf("\tCodec %s ID %d bit_rate %lld\n", pLocalCodec->name, pLocalCodec->id, pLocalCodecParameters->bit_rate);
  }

  if (video_stream_index == -1) {
    printf("File %s does not contain a video stream!\n", argv[1]);
    return -1;
  }

  // https://ffmpeg.org/doxygen/trunk/structAVCodecContext.html
  AVCodecContext *pCodecContext = avcodec_alloc_context3(pCodec);
  if (!pCodecContext)
  {
    printf("failed to allocated memory for AVCodecContext\n");
    return -1;
  }

  // Fill the codec context based on the values from the supplied codec parameters
  // https://ffmpeg.org/doxygen/trunk/group__lavc__core.html#gac7b282f51540ca7a99416a3ba6ee0d16
  if (avcodec_parameters_to_context(pCodecContext, pCodecParameters) < 0)
  {
    printf("failed to copy codec params to codec context\n");
    return -1;
  }

  // Initialize the AVCodecContext to use the given AVCodec.
  // https://ffmpeg.org/doxygen/trunk/group__lavc__core.html#ga11f785a188d7d9df71621001465b0f1d
  if (avcodec_open2(pCodecContext, pCodec, NULL) < 0)
  {
    printf("failed to open codec through avcodec_open2\n");
    return -1;
  }

  AVPacket *pPacket = av_packet_alloc();

  printf("time_base=%d/%d\n", pFormatContext->streams[video_stream_index]->time_base.den, pFormatContext->streams[video_stream_index]->time_base.num);
  double time_base = (double)pFormatContext->streams[video_stream_index]->time_base.den / pFormatContext->streams[video_stream_index]->time_base.num;

  int frame_counter = 0;
  // fill the Packet with data from the Stream
  // https://ffmpeg.org/doxygen/trunk/group__lavf__decoding.html#ga4fdb3084415a82e3810de6ee60e46a61
  while (av_read_frame(pFormatContext, pPacket) >= 0) {
    if (pPacket->flags > 1)
      printf("Found invalid packet!!!\n");

    for (int i = 0; i < 4096; i += 4) {
      uint32_t* slice_qp_delta = (uint32_t*)(pPacket->data + i);
      printf("%b ", *slice_qp_delta);
      // printf("%b", *slice_qp_delta);
    }
    printf("\n");
    printf("%.4f,%05d\n", pPacket->pts/time_base, pPacket->size);
#if 0
    char type;
    if (pPacket->flags & AV_PKT_FLAG_KEY) {
      type = 'I';
    } else {
      type = 'P';
    }
    printf("%c-Frame: %3d with size %d dur=%lu\n", type, frame_counter, pPacket->size, pPacket->duration);

    printf("data=%d\n", ((uint64_t)pPacket->data)&0xFFFFFFF);

    struct NAL_Unit nal;
    parse_nal_unit(&nal, pPacket->data, pPacket->size);

    int i;
    for (i = 0; i < 64; i++){
      if (i > 0) printf(" ");
      printf("%02X", pPacket->data[i]);
    }
    printf("\n");
    if (frame_counter == 0) {
      printf("NAL: %d %d\n", pPacket->data[4]&0x1F, (pPacket->data[4]>>5)&3);
    } else {
      printf("NAL: %d %d\n", pPacket->data[3]&0x1F, (pPacket->data[3]>>5)&3);
    }
    // printf("type=%d start_code_prefix_len=%d\n", nal.type, nal.start_code_prefix_len);
#endif

    frame_counter++;
    if (frame_counter == 4)
      break;
  }

  printf("\n\n\n\n\nreleasing all the resources\n");

  avformat_close_input(&pFormatContext);
  av_packet_free(&pPacket);
  avcodec_free_context(&pCodecContext);
#endif
  return 0;
}
