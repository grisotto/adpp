# Copyright 2017, Gurobi Optimization, Inc.

PLATFORM = linux64
INC      = $(GUROBI_HOME)/include/
CPP      = g++
CARGS    = -m64 -g -O3
CPPLIB   = -L$(GUROBI_HOME)/lib/ -lgurobi_c++ -lgurobi70

all:	
	$(CPP) $(CARGS) packing.cpp -o packing -I$(INC) $(CPPLIB) -lm


