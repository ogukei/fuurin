# compiles all the GLSL files in ./data

ifndef GLSLC
GLSLC=glslangValidator
endif

SOURCE_DIR=.
BUILD_DIR=.

SOURCES_VERT=$(shell find $(SOURCE_DIR) -name '*.vert')
SOURCES_FRAG=$(shell find $(SOURCE_DIR) -name '*.frag')
SOURCES_RGEN=$(shell find $(SOURCE_DIR) -name '*.rgen')
SOURCES_RMISS=$(shell find $(SOURCE_DIR) -name '*.rmiss')
SOURCES_RCHIT=$(shell find $(SOURCE_DIR) -name '*.rchit')
SOURCES_RINT=$(shell find $(SOURCE_DIR) -name '*.rint')
SOURCES_RAHIT=$(shell find $(SOURCE_DIR) -name '*.rahit')
SOURCES=$(SOURCES_VERT) $(SOURCES_FRAG) $(SOURCES_RGEN) $(SOURCES_RMISS) $(SOURCES_RCHIT) $(SOURCES_RINT) $(SOURCES_RAHIT)

OBJECTS_0=$(patsubst $(SOURCE_DIR)/%.vert, $(BUILD_DIR)/%.vert.spv, $(SOURCES))
OBJECTS_1=$(patsubst $(SOURCE_DIR)/%.frag, $(BUILD_DIR)/%.frag.spv, $(OBJECTS_0))
OBJECTS_2=$(patsubst $(SOURCE_DIR)/%.rgen, $(BUILD_DIR)/%.rgen.spv, $(OBJECTS_1))
OBJECTS_3=$(patsubst $(SOURCE_DIR)/%.rmiss, $(BUILD_DIR)/%.rmiss.spv, $(OBJECTS_2))
OBJECTS_4=$(patsubst $(SOURCE_DIR)/%.rchit, $(BUILD_DIR)/%.rchit.spv, $(OBJECTS_3))
OBJECTS_5=$(patsubst $(SOURCE_DIR)/%.rint, $(BUILD_DIR)/%.rint.spv, $(OBJECTS_4))
OBJECTS_6=$(patsubst $(SOURCE_DIR)/%.rahit, $(BUILD_DIR)/%.rahit.spv, $(OBJECTS_5))
OBJECTS=$(OBJECTS_6)

INCLUDE=$(shell find $(SOURCE_DIR) -name '*.glsl')

all: $(OBJECTS)
clean:
	rm -f $(BUILD_DIR)/*.spv

$(BUILD_DIR)/%.vert.spv: $(SOURCE_DIR)/%.vert $(INCLUDE)
	$(GLSLC) --target-env vulkan1.2 \
	-c $< \
	-o $@

$(BUILD_DIR)/%.frag.spv: $(SOURCE_DIR)/%.frag $(INCLUDE)
	$(GLSLC) --target-env vulkan1.2 \
	-c $< \
	-o $@

$(BUILD_DIR)/%.rgen.spv: $(SOURCE_DIR)/%.rgen $(INCLUDE)
	$(GLSLC) --target-env vulkan1.2 \
	-c $< \
	-o $@

$(BUILD_DIR)/%.rmiss.spv: $(SOURCE_DIR)/%.rmiss $(INCLUDE)
	$(GLSLC) --target-env vulkan1.2 \
	-c $< \
	-o $@

$(BUILD_DIR)/%.rchit.spv: $(SOURCE_DIR)/%.rchit $(INCLUDE)
	$(GLSLC) --target-env vulkan1.2 \
	-c $< \
	-o $@

$(BUILD_DIR)/%.rint.spv: $(SOURCE_DIR)/%.rint $(INCLUDE)
	$(GLSLC) --target-env vulkan1.2 \
	-c $< \
	-o $@

$(BUILD_DIR)/%.rahit.spv: $(SOURCE_DIR)/%.rahit $(INCLUDE)
	$(GLSLC) --target-env vulkan1.2 \
	-c $< \
	-o $@
