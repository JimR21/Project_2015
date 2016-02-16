CC=g++
CFLAGS=-c -g  -w -std=c++0x -O3 -march=native -flto
LDFLAGS= -pthread
SOURCES=main.cpp Journal.cpp JournalRecord.cpp Gen_HashTable.cpp Key_HashTable.cpp Val_HashTable.cpp Tid_HashTable.cpp Bucket.cpp BucketData.cpp BucketVal.cpp Val_bdata.cpp ValidationIndex.cpp valClass.cpp queryClass.cpp columnClass.cpp bitset.cpp Val_listbucket.cpp ValidationNode.cpp
OBJECTS=$(SOURCES:.cpp=.o)
DEPS=Journal.hpp JournalRecord.hpp Gen_HashTable.hpp Key_HashTable.hpp Tid_HashTable.hpp Bucket.hpp BucketData.hpp mainStructs.hpp bitset.hpp ValidationNode.hpp
EXECUTABLE=test.out

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(SOURCES) $(DEPS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) *.o *.gch

stats:
	g++  -std=c++0x statsmain.cpp Journal.cpp JournalRecord.cpp Gen_HashTable.cpp Key_HashTable.cpp Tid_HashTable.cpp Bucket.cpp BucketData.cpp ValidationNode.cpp -pthread -o statsmain.out
