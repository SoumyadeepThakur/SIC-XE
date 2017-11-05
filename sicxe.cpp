#include <iostream>
#include <cstdlib>
#include "sicxe_asm.h"
using namespace std;

int main(int argc, char const *argv[])
{
	string filename = argv[1];
	string dump = argv[2];
	assembler asmb(filename,dump);
	asmb.assemble();
	
	return 0;
}