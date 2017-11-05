#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <random>
#include <iomanip>
#include "sicxe_asm.h"
#include "tables.h"
#define MEMORY_SIZE 1048576
using namespace std;

unsigned char memory[MEMORY_SIZE]; // model for the memory
void execute_code(string);
std::map <std::string, std::pair<int, int> > op_tab;
std::map <std::string, int> reg_tab;

assembler::assembler(string infile, string outfile):ip_file(infile),op_file(outfile)
{
	code=0;
	code_start=0;
	instr_end=0;
	main_end=0;
	inter_file = "inter.txt";
	init_operator_table();
	init_register_table();
	clear_memory();
}
unsigned char *assembler::byte_format(string byte_string)
{
	// byte string should be of the form F'XX' where F is the format (C,X,B) and XX is the value
	// e.g C'Z'; X'A2'; B'00011011'
	char beg_char = byte_string.at(0);
	unsigned char *bytearr;
	int len = byte_string.size();
	string buf = byte_string.substr(2,len-1);
	try
	{
		switch (beg_char)
		{
			case 'C': bytearr = new unsigned char[len-3];
					  for (int i = 0; i<len-3; i++)
					  {
						  bytearr[i] = (unsigned char)(buf[i]);
						  memory[code++] = bytearr[i];
					  }
					  break;
			case 'X': bytearr = new unsigned char[(len-3)/2];
					  for (int i=0; i<len-3; i+=2)
					  {
						  bytearr[i] = (unsigned char)(std::stoi(buf.substr(i,i+2)),nullptr,16);
						  memory[code++] = bytearr[i];
					  }
		}
	}
	catch ( ... )
	{
		cout << "Invalid byte format" << endl;
		return NULL;
	}
	return bytearr;
}
int assembler::data_directive(std::string opcode, std::string operand)
{
	if (!opcode.compare("WORD"))
		return 3;
	else if (!opcode.compare("RESW"))
	{
		return 3*(std::stoi(operand, nullptr, 16));
	}	
	else if (!opcode.compare("RESB"))
	{
		return (std::stoi(operand, nullptr, 16));
	}
	else if (!opcode.compare("BYTE"))
	{
		char type = operand.at(0);
		int bytelength;
		if (type == 'C')
		{
			bytelength = operand.substr(2,operand.length()-3).length();
		}
		else if (type == 'X')
		{
			bytelength = (operand.substr(2,operand.length()-3).length()+1)/2;	
		}
		return bytelength;
	}
	return 0;
}
void assembler::pass1()
{
	std::fstream in(ip_file,ios::in);
	std::fstream inter(inter_file, ios::out);
	std::string line; int lineno=0;
	std::random_device rd; // obtain a random number from hardware
    std::mt19937 eng(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, 1024); // define the range
    //code_start = distr(eng);
    code = code_start;
    int start, locctr, length, size;
    std::string name;
	std::string delimiter = "	";
	
	//tokenize file
	while (!in.eof())
	{
		//cout << "---LINE---" << endl;
		getline(in,line);
		std::string tokens[3];
		std::string temp_line = line;
		size_t pos = 0;
		int count=0;
		bool extended = false; // extended instruction format 
		
		while ((pos = line.find(delimiter)) != std::string::npos) 
		{
			tokens[count++] = line.substr(0, pos);
    		line.erase(0, pos + delimiter.length());
		}
		//cout << "COUNT---" << count << endl;
		tokens[count++] = line;

		int p=0; //string word;
		cout << tokens[0] << "," << tokens[1] <<  "," << tokens[2] << endl;
		if (!tokens[1].compare("END"))
		{
			code_length = locctr - start;
			inter << "	" << temp_line << endl;
			break;
		}
		if (!tokens[1].compare("START"))
		{
			start = std::stoi(tokens[2], nullptr, 16);
			locctr = start;
			name = tokens[0];
			cout << locctr << "-" << name << endl;
			inter << "0000	";
		}
		else
		{
			inter << locctr << "	";
			if (tokens[0].compare("")) // LABEL present
			{
				if (sym_tab.find(tokens[0]) != sym_tab.end())
				{
					// ERROR: Repetition of labels
					cout << "ERROR: Label repeated" << endl;
					sym_tab[tokens[0]].second = false; 
				}
				else
				{
					sym_tab[tokens[0]].first = locctr;
					sym_tab[tokens[0]].second = true;
				}
			}
			if (tokens[1].at(0) == '+') // extended addressing format used here
			{
				extended = true;
				tokens[1].erase(tokens[1].begin());
			}
			
			if (op_tab.find(tokens[1])!=op_tab.end())
			{
				locctr += op_tab[tokens[1]].second; // change here
				locctr += (extended)?1:0;
			}
			else if (size = data_directive(tokens[1], tokens[2]))
			{
				locctr += size;
			}
			else
			{	
				cout << "Invalid operation code" << endl; 
			}
		}
		inter << temp_line << endl;

	}
	inter.close();
	in.close();
	
	//std::string t;
	cout << "Symtab" << endl;
	/*
	for (map<std::string, std::pair <int, bool> >::iterator it = sym_tab.begin(); it!=sym_tab.end(); ++it)
	{
		cout  << " " << it->first << " " << (it->second).first << " " << (it->second).second << endl;
	}
	*/
	for (auto& t : sym_tab)
	{
		cout << t.first << " => " << t.second.first << " " << t.second.second << endl;
	}
	

	cout << "Assembler Pass 1 finished successfully" << endl;

}

void assembler::pass2()
{
	std::fstream inter("inter.txt", ios::in);
	std::fstream out("op_file", ios::out);
	std::fstream listing("listing.txt", ios::out);
	std::string line, name;
	std::string delimiter = "	";
	int start, locctr=0, lineno=5;
	bool error_flag = false; // CHECK IF ERROR IN ASSEMBLY

	/*
	ASSUMING ONLY PROGRAM COUNTER RELATIVE ADDRESSING, NOT BASE RELATIVE
	*/
	
	listing << "LINE" << "	" << "LOC" << "	" << "SOURCE STATEMENT" << "	" << "OBJECT CODE" << endl;
	while (!inter.eof())
	{
		getline(inter, line);
		//cout << line << endl;
		std::string tokens[4];
		std::string temp_line = line;
		size_t pos = 0;
		int count=0;
		bool extended = false; // extended instruction format 

		listing << lineno << "	" << temp_line << "	";
		lineno += 5;
		
		while ((pos = line.find(delimiter)) != std::string::npos) 
		{
			tokens[count++] = line.substr(0, pos);
    		line.erase(0, pos + delimiter.length());
		}
		tokens[count++] = line;

		cout <<  "Read: " << tokens[0] << " " << tokens[1] << " " << tokens[2] << " " << tokens[3] << endl;
		if (!tokens[2].compare("END"))
		{
			out << "E" << std::setw(6) << std::setfill('0') << std::hex << start << endl;
			listing << endl;
			break;
		}
		else if (!tokens[2].compare("START"))
		{
			start = std::stoi(tokens[3], nullptr, 16);
			locctr = start;
			name = tokens[1];
			name.resize(8,' ');
			cout << locctr << "-" << name << endl;
			out << "H" << name << std::setw(6) << std::setfill('0') << std::hex << start << std::setw(6) << std::setfill('0') << std::hex << code_length << endl;	
			listing << "000000" << endl;
		}
		else
		{
			
			int opcode, op_addr, addr, hexcode, i=1, x=0, n=1;
			std::string objcode="";
			if (tokens[2].at(0) == '+') // extended addressing format used here
			{
				extended = true;
				tokens[2].erase(tokens[2].begin());
			}
			if (op_tab.find(tokens[2]) != op_tab.end())
			{
				// OPERATOR FOUND
				opcode = op_tab[tokens[2]].first;
				int instrbytes = op_tab[tokens[2]].second;
				instrbytes += (extended)?1:0;

				if (instrbytes == 1)
				{
					// NO OPERANDS NEEDED
					locctr += 1;
					out << std::setw(2) << std::setfill('0') << std::hex << opcode << endl;
					listing << std::setw(2) << std::setfill('0') << std::hex << opcode << endl;
				}
				else if (instrbytes == 2)
				{
					string r1, r2;
					int rx1, rx2;
					int pos;
					locctr += 2;
					if (pos = tokens[3].find(",") != std::string::npos)
					{
						r1 = tokens[3].substr(0,pos);
						r2 = tokens[3].substr(pos+1);
						rx1 = reg_tab[r1];
						rx2 = reg_tab[r2];
						//cout << "Reg: " << r1 << "-" << r2 << endl; 
						hexcode = opcode << 8;
						hexcode |= (rx1 << 4);
						hexcode |= rx2;
					}
					else
					{
						rx1 = reg_tab[tokens[3]];
						rx1 = rx1 << 4;
						hexcode = (opcode << 8) | rx1;
					}
					out << setw(4) << setfill('0') << std::hex << hexcode << endl;
					listing << setw(4) << setfill('0') << std::hex << hexcode << endl;
				}
				else
				{
					if (!(tokens[3].substr(tokens[3].length()-2, 2).compare(",X")))
					{
						
						x = 1; //INDEXED ADDRESSING
						tokens[3].erase(tokens[3].end()-1);
						tokens[3].erase(tokens[3].end()-1);
					}
					if (tokens[3].at(0) == '@')
					{
						//INDIRECT ADDRESSING
						tokens[3].erase(tokens[3].begin());
						opcode |= 2; // i=1
						i=0;					
					}

					if (sym_tab.find(tokens[3]) != sym_tab.end())
					{
						//OPERAND FOUND AS WELL
						//ASSUMING PC REL ADDRESSING
						op_addr = sym_tab[tokens[3]].first;
						//opcode |= 3; // n=i=1;

					}
					else if (tokens[3].at(0) == '#')
					{
						//IMMEDIATE OPERAND
						tokens[3].erase(tokens[3].begin());
						op_addr = std::stoi(tokens[3],nullptr, 16);
						opcode |= 1; // i=1
						n=0;

					}
					if (n==1 && i==1) opcode |= 3;
					hexcode = opcode;
					cout << "HEXCODE: " << hexcode << endl;
					if (instrbytes == 4)
					{
						if (op_addr >= 1048576)
						{
							cout << "Address out of range" << endl; 
							error_flag = true;
						}
						locctr += 4;
						hexcode = (hexcode << 24) + op_addr;
						hexcode |= 1048576;
					}
					else if (instrbytes == 3 )
					{
						// memory operations
						// 3 bytes, i.e not extended memory
						if (n==0)
						{
							//IMMEDIATE ADDRESSING
							addr = op_addr;
						}
						else
						{	
							int rel_addr = op_addr - locctr - 3;
							cout << "Khargosh: " << rel_addr;
							if (rel_addr < 0)
								rel_addr += 4096;

							if (rel_addr < 0 || rel_addr >= 4096)
							{
								cout << "Address out of range" << endl; 
							}
							else
							{
								addr = rel_addr;
							}
						}
						locctr += 3;
						//cout << "ABCD: " << (op_addr - locctr) << endl;
						hexcode = hexcode << 16;
						//cout << "HEXCODE AFTER SHIFT: " << std::hex << hexcode << endl;
						hexcode |= addr; // PC relative
						//cout << "HEXCODE AFTER ORING" << std::hex << hexcode << endl;
						if (x==1)
							hexcode |= 32768;
						out << std::setw(6) << std::setfill('0') << std::hex << hexcode << endl;
						listing << std::setw(6) << std::setfill('0') << std::hex << hexcode << endl;
					}
				}

				//if (x==1) hexcode |= 8388608;

			} 
			/*
			else if (!tokens[2].compare("RESW"))
			{
				int val = std::stoi(tokens[3],nullptr,16);
				while(val--)
				out << "000000" << endl;
				locctr += 3;
			}
			else if (!tokens[2].compare("RESB"))
			{
				out << ""
			}
			*/

		}
		
	}
	listing.close();
	out.close();
	inter.close();
}

int assembler::get_data_location(string name, string st_type="", string val="")
{ // allocate the variable to somewhere in memory
	if (!st_type.compare("WORD")) // allocate constant word
	{
		//cout << "word" << val << endl;
		int intval = std::stoi(val,nullptr,16);
		unsigned char byte1 = (intval >> 16);
		unsigned char byte2 = ((intval & 65280) >> 8);
		unsigned char byte3 = (intval & 255);
		//cout << "bytes" << (int)byte1 << (int)byte2 << (int)byte3  << endl;
		int loc = code;
		memory[code++] = byte3;
		memory[code++] = byte2;
		memory[code++] = byte1;
		return loc;
	}
	else if (!st_type.compare("RESB") || !st_type.compare("RESW"))
	{
		//cout << "res" << endl;
		int bytes = std::stoi(val,nullptr,16);
		if (!st_type.compare("RESW")) bytes*=3;
		int loc = code;
		code+=bytes;
		return loc;
	}
	else if (!st_type.compare("BYTE"))
	{
		//cout << "byte" << endl;
		int loc = code;
		unsigned char *bytes = byte_format(val);


		return loc;
	}
}
void assembler::dump_code()
{
	fstream outf(op_file,ios::out);
	outf << "H" << std::setw(4) << std::setfill('0') << std::hex << code_start << std::setw(4) << std::setfill('0') << std::hex << (instr_end - code_start) << std::setw(4) << std::setfill('0') << std::hex << (code-code_start) << std::setw(4) << std::setfill('0') << std::hex << main_end << endl;
	for (int i=code_start; i<code; i++)
	{
		outf << std::setw(2) << std::setfill('0') << std::hex << (int)(memory[i]) << endl;
	}
	outf.close();
}
void assembler::clear_memory()
{
	for (int i=0; i<MEMORY_SIZE; i++) memory[i]=0;
}
void assembler::assemble()
{
	pass1();
	pass2();
	//dump_code();
}
