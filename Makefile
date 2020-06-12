all: test

test: data.h
	g++ -Wno-format-security test.cc -I$(HOME)/dev -o test
