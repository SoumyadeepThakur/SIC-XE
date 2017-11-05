#include <iostream>
#include <cstdlib>
#include "tables.h"
using namespace std;

void init_operator_table()
{
	op_tab["ADD"] = std::make_pair(24,3);
	op_tab["ADDR"] = std::make_pair(144,2);
	op_tab["AND"] = std::make_pair(48,3);
	op_tab["CLEAR"] = std::make_pair(180,2);
	op_tab["SUB"] = std::make_pair(28,3);
	op_tab["MUL"] = std::make_pair(32,3);
	op_tab["DIV"] = std::make_pair(36,3);
	op_tab["LDA"] = std::make_pair(00,3);
	op_tab["LDX"] = std::make_pair(04,3);
	op_tab["STA"] = std::make_pair(12,3);
	op_tab["STX"] = std::make_pair(16,3);
	op_tab["COMP"] = std::make_pair(40,3);
	op_tab["TIX"] = std::make_pair(44,3);
    op_tab["RSUB"] = std::make_pair(76,3);
    op_tab["JSUB"] = std::make_pair(72,3);
    op_tab["J"] = std::make_pair(60,3);
	op_tab["JEQ"] = std::make_pair(48,3);
	op_tab["JGT"] = std::make_pair(52,3);
	op_tab["JLT"] = std::make_pair(56,3);
}

void init_register_table()
{
	reg_tab["A"] = 0;
	reg_tab["X"] = 1;
	reg_tab["L"] = 2;
	reg_tab["B"] = 3;
	reg_tab["S"] = 4;
	reg_tab["T"] = 5;
	reg_tab["F"] = 6;
	reg_tab["PC"] = 8;
	reg_tab["SW"] = 9;

}