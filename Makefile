CXXFLAGS=-O -g -fPIC -std=c++11 -Wall
CXXFLAGS+=`llvm-config-3.2 --cflags`
#CXXFLAGS+=-I/home/i/src/llvm/include

all: clearstack.so stack_ptr stack_ptr_cs

clearstack.so: clearstack.o
	$(CXX) -shared $< -o $@

stack_ptr: stack_ptr.c
	$(CC) -g $< -o $@ -lgc -O2

stack_ptr_cs: stack_ptr.c clearstack.so
	./clang_wrap.rb $< -c -o stack_ptr_cs.o -g && gcc stack_ptr_cs.o -o $@ -lgc -O2

clean:
	rm -f clearstack.so *.o
