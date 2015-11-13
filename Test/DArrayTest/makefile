CC=g++
CFLAGS=-c -Wall -g -std=c++0x
LDFLAGS=
SOURCES=darraymain.cpp Bucket.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=test.out

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *o all
