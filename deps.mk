VERT_SHADERS = $(wildcard shaders/*.vert)
FRAG_SHADERS = $(wildcard shaders/*.frag)
VERT_SHADER_TARGETS = $(patsubst shaders/%.vert, shaders/%.vert.test,	\
$(VERT_SHADERS))

FRAG_SHADER_TARGETS = $(patsubst shaders/%.frag, shaders/%.frag.test,	\
$(FRAG_SHADERS))

LIBS += $(shell sdl2-config --libs) -lm -ldl

CFLAGS += -g $(shell sdl2-config --cflags)
WARNINGS += -Wno-documentation
all: engine $(VERT_SHADER_TARGETS) $(FRAG_SHADER_TARGETS)

engine: linux-platform.o glad.o rgl.o rutils/math.o rutils/file.o rutils/string.o
	$(CC) $(LDFLAGS) -o $@ $^
