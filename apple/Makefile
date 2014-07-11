TARGET = iPodAccess
SOURCES = $(wildcard *.c)
OBJS := $(patsubst %.c,%.o, $(SOURCES))

CC = arm-none-linux-gnueabi-gcc
STRIP = arm-none-linux-gnueabi-strip
CFLAGS = -static -I./
LDFLAGS =

all: $(TARGET) 

%.d: %.c
	@set -e; rm -f $@; \
		$(CC) -MM  $< > $@.bak; \
		sed 's|.*|$@ &|g' < $@.bak > $@; \
		rm -f $@.bak

-include $(SOURCES:.c=.d)

$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)
	$(STRIP) $@

clean:
	rm -f *.o $(TARGET) *.d


