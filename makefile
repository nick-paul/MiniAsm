CXX=g++
CXXFLAGS=-std=c++14

src = $(wildcard src/*.cpp)
obj = $(src:.cpp=.o)

miniasm: $(obj)
	$(CXX) -o $@ $^ $(CXXFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) myprog
