


cppsrc := $(wildcard *.cpp)

cppo := $(cppsrc:%.cpp=%.o)

linkflags=-lglfw $(shell pkg-config --libs glew)

CXXFLAGS=-O0 -g

all: balash

balash: $(cppo)
	$(CXX) $(cppo) -o $@ $(linkflags) 
