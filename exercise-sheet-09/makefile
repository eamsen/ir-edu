CXX:=g++ -std=c++0x
CFLAGS:=-O3 -Wall -fopenmp
LIBS:=-lrt
TESTLIBS:=-lgtest -lgtest_main -lpthread $(LIBS)
MAIN_BINARIES:=$(basename $(wildcard *main.cc))
TEST_BINARIES:=$(basename $(wildcard *test.cc))
HEADER:=$(wildcard *.h)
OBJECTS:=index.o query-processor.o k-means-clustering.o

.PRECIOUS: %.o

all: compile test checkstyle

compile-all: compile compile-test

compile: $(MAIN_BINARIES)

compile-test: $(TEST_BINARIES)

profile: CFLAGS=-Wall -O3 -DPROFILE
profile: LIBS+=-lprofiler
profile: clean compile

opt: CFLAGS=-Ofast -flto -fopenmp -mtune=native -DNDEBUG -Wall
opt: clean compile

debug: CFLAGS=-O0 -g
debug: compile

test: $(TEST_BINARIES)
	@for T in $(TEST_BINARIES); do ./$$T; done

checkstyle:
	@python ../cpplint.py *.h *.cc

clean:
	@rm -f *.o
	@rm -f $(MAIN_BINARIES)
	@rm -f $(TEST_BINARIES)
	@rm -f *.class
	@rm -f *Test.TMP.*
	@rm -f core
	@echo "cleaned"

%main: %main.o $(OBJECTS)
	@$(CXX) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo "build $@"

%test: %test.o $(OBJECTS)
	@$(CXX) $(CFLAGS) -o $@ $^ $(TESTLIBS)
	@echo "build $@"

%.o: %.cc $(HEADER)
	@$(CXX) $(CFLAGS) -c $<
	@echo "compiled $<"
