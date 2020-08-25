all: test

test: test.cc ctk.h math.h data.h memory.h
	g++ -g -Wno-format-security test.cc -I$(HOME)/dev -o test
