#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
using namespace std;

int main(int argc, char const *argv[])
{
	std::fstream fs("sample.txt", ios::in);
	std::string s;
	getline(fs,s);
	//getline(fs,s);
	//getline(fs,s);
	//getline(fs,s);
	std::string delimiter = "	";
	cout << s.length() << endl;
	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		cout << pos << "-";
    	token = s.substr(0, pos);
    	std::cout << token << std::endl;
    	s.erase(0, pos + delimiter.length());
	}
	std::cout << "REM:" << s << std::endl;

	return 0;
}