CC=g++
CFLAGS=-c -Wall -g -std=c++0x -O2
LDFLAGS=
SOURCES=main.cpp Journal.cpp JournalRecord.cpp Bucket.cpp Gen_HashTable.cpp Key_HashTable.cpp BucketData.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=test.out

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o *.gch
