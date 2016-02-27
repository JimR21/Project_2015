CC=g++
CFLAGS=-c -g  -w -std=c++0x -O3 -march=native -flto
LDFLAGS= -pthread
SOURCES=main.cpp Journal.cpp JournalRecord.cpp Gen_HashTable.cpp Key_HashTable.cpp Val_HashTable.cpp Tid_HashTable.cpp Bucket.cpp BucketData.cpp BucketVal.cpp Val_bdata.cpp valClass.cpp queryClass.cpp columnClass.cpp bitset.cpp Val_listbucket.cpp ValidationNode.cpp Threads/ThreadInterface.cpp
OBJECTS=$(SOURCES:.cpp=.o)
DEPS=Journal.hpp JournalRecord.hpp Gen_HashTable.hpp Key_HashTable.hpp Val_HashTable.hpp Tid_HashTable.hpp Bucket.hpp BucketData.hpp BucketVal.hpp Val_bdata.hpp valClass.hpp queryClass.hpp columnClass.hpp mainStructs.hpp bitset.hpp Val_listbucket.hpp ValidationNode.hpp Threads/ThreadInterface.hpp Threads/Thread.hpp
EXECUTABLE=test.out

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(SOURCES) $(DEPS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) *.o *.gch
	$(RM) Threads/*.o
	$(RM) Threads/*.gch

stats:
	g++  -std=c++0x statsmain.cpp Journal.cpp JournalRecord.cpp Gen_HashTable.cpp Key_HashTable.cpp Tid_HashTable.cpp Bucket.cpp BucketData.cpp ValidationNode.cpp -pthread -o statsmain.out
