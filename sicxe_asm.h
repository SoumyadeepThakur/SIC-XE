/*
AUTHOR: SOUMYADEEP THAKUR
B.E 3RD YEAR
JADAVPUR UNIVERSITY
DATE: 3 NOV 2017
*/

/* guard */
#ifndef __SICXE_ASM_H_INCLUDED__
#define __SICXE_ASM_H_INCLUDED__

#ifndef __DATA_STRUCTURES_INCLUDED__
#define __DATA_STRUCTURES_INCLUDED__

#include <vector>
#include <map>
#include <set>

#endif

#ifndef __STRING_INCL__
#define __STRING_INCL__

#include <string>

#endif


/*util function to check for branches */
//std::map <std::string, int> op_tab;
bool is_branch_instr(std::string);
/*class definition */
class assembler
{
	std::string ip_file, op_file, inter_file;
	int code_start, code, instr_end, main_end;
	int code_length;
	std::map <std::string, std::pair<int, bool> > sym_tab;
	//std::map <std::string, int> label_table;
	//std::set <std::string> labels, symbols;
	bool pass1();
	bool pass2();
	int data_directive(std::string, std::string);
	int get_data_location(std::string,std::string,std::string);
	unsigned char *byte_format(std::string);
	void dump_code();

	public:
	assembler(std::string,std::string);
	void clear_memory();
	void load_memory();
	void assemble();

};

#endif
