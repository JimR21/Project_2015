CC=g++
CFLAGS=-c -Wall -g -std=c++0x -Ofast
LDFLAGS=
SOURCES=main.cpp Journal.cpp JournalRecord.cpp Gen_HashTable.cpp Key_HashTable.cpp Tid_HashTable.cpp Bucket.cpp BucketData.cpp Val_HashTable.cpp Val_bdata.cpp
OBJECTS=$(SOURCES:.cpp=.o)
DEPS=Journal.hpp JournalRecord.hpp Gen_HashTable.hpp Key_HashTable.hpp Tid_HashTable.hpp Bucket.hpp BucketData.hpp
EXECUTABLE=test.out

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(SOURCES) $(DEPS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o *.gch
