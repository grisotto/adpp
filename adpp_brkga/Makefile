# Compiler binary:
ifeq ($(shell uname), Darwin)
	PLATFORM = mac64
	CPP      = clang++
	GPP      = g++-7
	CC       = clang
	CARGS    = -std=c++1y -m64 -O3 
	RELEASE := $(shell uname -r | cut -f 1 -d .)
	ifneq ($(RELEASE), 11)
		CPPSTDLIB = -stdlib=libstdc++ 
	endif
	MV = mv *.dSYM temp/  
else
	PLATFORM = linux64
	CPP      = g++
	CFLAGS= -std=c++14 -m64 -fopenmp -g
endif
# Recommended compiler flags for speed:
#	OpenMP enabled
#	full binary code optimization
#	full error and warning reports
#	no range checking within BRKGA:

# Compiler flags for debugging; uncomment if needed:
#	range checking enabled in the BRKGA API
#	OpenMP disabled
#	no binary code optimization
#CFLAGS= -DRANGECHECK -Wextra -Wall -Weffc++ -ansi -pedantic -Woverloaded-virtual -Wcast-align -Wpointer-arith

# Objects:
	OBJECTS=Population.o brkga2dkDecoder.o brkga2dkDecoder_4.o brkga2dk.o knapsack2d.o figura.o 2dbinpack.o 

# Targets:
all: brkga2dk 

%.o: %.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

Population.o: brkgaAPI/Population.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

figura.o: 2dbinpack/figura.c
	$(CPP) $(CFLAGS) -c $< -o $@

2dbinpack.o: 2dbinpack/2dbinpack.c
	$(CPP) $(CFLAGS) -c $< -o $@

brkga2dk: $(OBJECTS)
	$(CPP) $(CFLAGS) $(OBJECTS) -o brkga2dk

clean:
	rm -f brkga2dk $(OBJECTS)
