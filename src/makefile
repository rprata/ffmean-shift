CXX :=g++

all: CXXFLAGS :=-Wall -O2 -D__STDC_CONSTANT_MACROS
debug: CXXFLAGS :=-g -Wall -O0 -D__STDC_CONSTANT_MACROS

LDLIBS := -I. -I./includes

LDLIBS +=-lavformat \
		 -lavcodec \
		 -lavdevice \
		 -lavutil \
		 -lswscale \
		 -ljpeg \
		 -lz \
		 -lm \
		 -lpthread

SOURCE :=main.cpp Camera.cpp Filter.cpp Draw.cpp
BIN :=ffmean-shift

LDLIBS += `sdl-config --cflags --libs`

all:
	$(CXX) -o $(BIN) $(CXXFLAGS) $(SOURCE) $(LDLIBS)

debug:
	$(CXX) -o $(BIN) $(CXXFLAGS) $(SOURCE) $(LDLIBS)

clean:
	rm -r $(BIN)
