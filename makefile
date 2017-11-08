sicxe: sicxe_asm.o tables.o sicxe.o
	g++ $(CFLAGS) -o sicxe sicxe_asm.o tables.o sicxe.o
sicxe_asm.o: sicxe_asm.h sicxe_asm.cpp
	g++ $(CFLAGS) -c sicxe_asm.cpp -std=c++14
tables.o: tables.h tables.cpp
	g++ $(CFLAGS) -c tables.cpp -std=c++14
sicxe.o: sicxe.cpp sicxe_asm.h tables.h
	g++ $(CFLAGS) -c sicxe.cpp -std=c++14
clean:
	rm -f *.o
