ifeq ($(shell which pkg-config), )
$(error You need to install pkg-config in order to compile this sources)
endif


CFLAGS += $(shell pkg-config fuse --cflags)  -std=gnu99 -g3 -Wall -Wextra
CFLAGS += -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26

LDFLAGS += $(shell pkg-config fuse --libs)




EXE = p6fs
SOURCES += fuse-main.o common.o logging.o  disk.o

HELLO += ./example/hello.o 


all: $(EXE) example 

$(EXE): $(SOURCES)
	$(CC) -o $@ $^ $(LDFLAGS)

example: $(HELLO)
	$(CC) -o hello $^ $(LDFLAGS)


.PHONY: clean

clean:
	-rm -f *.o $(EXE)
	-rm hello

