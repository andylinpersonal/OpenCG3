CXX=g++
CXXFLAGS=-std=c++11 -O3 -Wall -Wextra -Wfatal-errors -pedantic
SRCDIR='../../src'

all: main

main:   main.o  StringParser.o  StringParser_ArgTree.o
	$(CXX) $(CXXFLAGS) -o main.elf main.o \
		$(SRCDIR)/StringParser.o \
		$(SRCDIR)/StringParser_ArgTree.o

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -o main.o -c main.cpp

StringParser.o: ../../src/StringParser.cpp
	$(CXX) $(CXXFLAGS) -o $(SRCDIR)/StringParser.o -c \
		$(SRCDIR)/StringParser.cpp

StringParser_ArgTree.o: ../../src/StringParser_ArgTree.cpp
	$(CXX) $(CXXFLAGS) -o $(SRCDIR)/StringParser_ArgTree.o -c \
		$(SRCDIR)/StringParser_ArgTree.cpp

clean:
	rm -f main.o main.elf \
		$(SRCDIR)/StringParser.o $(SRCDIR)/StringParser_ArgTree.o
