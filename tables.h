#ifndef __TABLES_H_INCLUDED__
#define __TABLES_H_INCLUDED__

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

#endif

/*
DECLARE STATIC TABLES REQUIRED FOR ASSEMBLY
*/

extern std::map <std::string, std::pair<int, int> > op_tab; // DECLARE OPERATOR TABLE
extern std::map <std::string, int> reg_tab; // DECLARE REGISTER TABLE

void init_operator_table();
void init_register_table();