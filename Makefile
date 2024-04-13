INCLUDES = $(shell pkg-config --cflags libavformat libavcodec libswresample libswscale libavutil sdl2)
LDFLAGS = $(shell pkg-config --libs libavformat libavcodec libswresample libswscale libavutil sdl2)

.PHONY: all

all: main

main: main.cpp imgui.cpp imgui_demo.cpp imgui_impl_sdlrenderer2.cpp imgui_tables.cpp imgui_widgets.cpp
	g++ $(INCLUDES) $(LDFLAGS) -g $^ -o main
