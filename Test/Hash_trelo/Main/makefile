CC=g++
CFLAGS=-c -Wall -g -std=c++0x
LDFLAGS=
SOURCES=main.cpp Journal.cpp JournalRecord.cpp Bucket.cpp HashTable.cpp BucketData.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=test.out

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o *.gch
