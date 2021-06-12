
# dependencies: libglfw3 libglfw3-dev libavformat-dev libavcodec-dev libglm-dev

TARGET=main

BUILD_DIR=build
SOURCE_DIR=src
INCLUDE_DIR=include

INCLUDES=-I$(SOURCE_DIR) -I$(INCLUDE_DIR)

CXX=g++
CXXFLAGS=-O2 \
	-pthread \
	-std=c++17 \
	$(INCLUDES)

LD=$(CXX)
LDFLAGS=-lpthread -lvulkan -lavformat -lavcodec

LIB_NVIDIA_VIDEO_PARSER_DIR=bin/libs/nv_vkvideo_parser/linux_amd64_release
LIB_NVIDIA_VIDEO_PARSER_RPATH=-Wl,-rpath=$(LIB_NVIDIA_VIDEO_PARSER_DIR)
LIB_NVIDIA_VIDEO_PARSER=-L$(LIB_NVIDIA_VIDEO_PARSER_DIR) -lnvidia-vkvideo-parser $(LIB_NVIDIA_VIDEO_PARSER_RPATH)

LIBS=$(shell pkg-config --libs glfw3) $(LIB_NVIDIA_VIDEO_PARSER)

SOURCES=$(shell find $(SOURCE_DIR) -name '*.cc')
OBJECTS=$(patsubst $(SOURCE_DIR)/%.cc, $(BUILD_DIR)/%.o, $(SOURCES))
HEADERS=$(shell find $(SOURCE_DIR) -name '*.h') $(shell find $(SOURCE_DIR) -name '*.inc')

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)
	rm -f $(TARGET)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cc $(HEADERS)
	@mkdir -p "$(@D)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(LD) -o $(TARGET) $(OBJECTS) $(LIBS) $(LDFLAGS)
