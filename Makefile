
# dependencies: libglfw3 libglfw3-dev libavformat-dev libavcodec-dev

TARGET=main

BUILD_DIR=build
SOURCE_DIR=src

INCLUDES=-I$(SOURCE_DIR)

CXX=g++
CXXFLAGS=-O2 \
	-pthread \
	-std=c++17 \
	$(INCLUDES)

LD=$(CXX)
LDFLAGS=-lpthread -lvulkan -lavformat -lavcodec

LIBS=$(shell pkg-config --libs glfw3)

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

$(TARGET): $(OBJECTS) $(LIBS)
	$(LD) -o $(TARGET) $(OBJECTS) $(LIBS) $(LDFLAGS)
