
CC = gcc
CFLAGS += -Wall -g -pedantic
LDFLAGS += -L/usr/local/lib -lfreetype
SOURCES=option.c app.c 
OBJECTS=$(SOURCES:.c=.o)
INCLUDE=-I/usr/include/freetype2/
EXECUTABLE=app_static

.PHONY: help all clean

all: $(SOURCES) $(EXECUTABLE)


$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< $(INCLUDE)

clean:
	rm -rf ./*.o ./app_static app.output
