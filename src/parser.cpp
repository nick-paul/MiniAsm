// MiniAsm.cpp : Defines the entry point for the console application.
//

#include <vector>
#include <iostream>
#include <map>
#include <stdlib.h>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <functional>
#include <cctype>
#include <locale>


#include "interpreter.h"

using namespace std;
using namespace miniasm;

const int OPSTATE_NE = 0;
const int OPSTATE_ADR = 1;
const int OPSTATE_LIT = 2;
const int OPSTATE_REG = 3;
const int OPSTATE_RDF = 4; //Reg deref

map<string, int> labels;

map<string, int> REG_NE;
map<string, int> REG_ADR;
map<string, int> ADR_ADR;
map<string, int> ADR_LIT;
map<string, int> RDF_ADR;
map<string, int> RDF_REG;
map<string, int> REG_RDF;
map<string, int> LIT_NE;
map<string, int> REG_LIT;
map<string, int> REG_REG;

void init_maps() {
	REG_NE["prt"] = PRT;
	REG_NE["prtch"] = PRTCH;
	REG_NE["inc"] = INC;
	REG_NE["dec"] = DEC;

	ADR_ADR["ld"] = LD_AA;

	REG_ADR["ld"] = LD_RA;

	ADR_LIT["ld"] = LD_AL;

	RDF_ADR["ld"] = LD_DA;

	RDF_REG["ld"] = LD_DR;

	REG_RDF["ld"] = LD_RD;

	LIT_NE["jp"] = JP;
	LIT_NE["exit"] = EXIT;

	REG_LIT["ld"] = LD_RL;
	REG_LIT["jnz"] = JNZ;

	REG_REG["ld"] = LD_RR;
	REG_REG["add"] = ADD;
	REG_REG["sub"] = SUB;
	REG_REG["mul"] = MUL;
	REG_REG["div"] = DIV;
	REG_REG["rem"] = REM;
	REG_REG["and"] = AND;
	REG_REG["or"] = OR;


}

// trim from start
static inline std::string &ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
	return ltrim(rtrim(s));
}

vector<string> split_str(string input, char c) {
	vector<string> arr;

	int current_line = 0;
	string current_str = "";
	for (int i = 0; i < input.length(); i++) {
		if (input[i] == c) {
			current_line++;
			arr.push_back(trim(current_str));
			current_str = "";
		}
		else {
			current_str += input[i];
		}
	}

	//Add the final string
	if (current_str != "") {
		arr.push_back(current_str);
	}

	return arr;
}

int get_safe(map<string, int> mp, string id) {
	map<string, int>::iterator it;
	it = mp.find(id);
	if (it != mp.end()) {
		return it->second;
	}
	else {
		throw err("operator does not exist");
	}
}

int get_instr(string s, int op1_s, int op2_s) {
	if (op1_s == OPSTATE_REG && op2_s == OPSTATE_NE) {
		return get_safe(REG_NE, s);
	}
	else if (op1_s == OPSTATE_LIT && op2_s == OPSTATE_NE) {
		return get_safe(LIT_NE, s);
	}
	else if (op1_s == OPSTATE_ADR && (op2_s == OPSTATE_ADR || op2_s == OPSTATE_REG)) {
		return get_safe(ADR_ADR, s);
	}
	else if (op1_s == OPSTATE_REG && op2_s == OPSTATE_ADR) {
		return get_safe(REG_ADR, s);
	}
	else if (op1_s == OPSTATE_REG && op2_s == OPSTATE_REG) {
		return get_safe(REG_REG, s);
	}
	else if (op1_s == OPSTATE_ADR && op2_s == OPSTATE_LIT) {
		return get_safe(ADR_LIT, s);
	}
	else if (op1_s == OPSTATE_REG && op2_s == OPSTATE_LIT) {
		return get_safe(REG_LIT, s);
	}
	else if (op1_s == OPSTATE_RDF && op2_s == OPSTATE_ADR) {
		return get_safe(RDF_ADR, s);
	}
	else if (op1_s == OPSTATE_RDF && op2_s == OPSTATE_REG) {
		return get_safe(RDF_REG, s);
	}
	else if (op1_s == OPSTATE_REG && op2_s == OPSTATE_RDF) {
		return get_safe(REG_RDF, s);
	}
	else {
		throw err("invalid argument types");
		return 0;
	}
	throw err("operator does not exist");
}


bool isNum(string s) {
	for (int i = 0; i < s.length(); i++) {
		if (s[i] < '0' || s[i] > '9') {
			return false;
		}
	}
	return true;
}

bool isAlpha(string s) {
	for (int i = 0; i < s.length(); i++) {
		char c = s[i];
		if (c < 'A' || (c > 'Z' && c < 'a') || c > 'z') {
			return false;
		}
	}
	return true;
}

void remove_comments(string& in) {
	int i = in.find_first_of(';');
	if (i != string::npos)
		in = in.substr(0, i);
	in = trim(in);
}

int get_op(string& opname, int& type) {
	if (opname == "") throw err("indent err: ");
	type = OPSTATE_NE;

	if (opname.length() == 1 && (opname[0] >= 'A' & opname[0] <= 'H')) {
		type = OPSTATE_REG;
		return opname[0] - 'A'; //The addr for the register
	}
	else if (isNum(opname)) {
		type = OPSTATE_LIT;
		return atoi(opname.c_str());

	}
	else if (isAlpha(opname)) {
		type = OPSTATE_LIT;
		map<string, int>::iterator ret = labels.find(opname);
		if (ret != labels.end()) {
			return ret->second;
		}
		else {
			throw err("Invalid label '" + opname + "'");
		}
	}
	else if (opname[0] == '&'){
		string opref = opname.substr(1, opname.length() - 1);
		if (opref.length() == 1 && (opref[0] >= 'A' & opref[0] <= 'H')) {
			type = OPSTATE_RDF;
			return opref[0] - 'A';
		}
		else if (isNum(opref)) {
			type = OPSTATE_ADR;
			return atoi(opname.substr(1, opname.length() - 1).c_str());
		}
		else {
			throw err("Invalid op ref: ");
		}
	}
	else {
		throw err("Invalid op: ");
	}
}

line parse_line(string in) { //&
	vector<string> strs = split_str(in, ' ');
	int num_of_items = strs.size();

	if (num_of_items <= 1) {
		throw err("err (" + in + ") Must have at at least one op");
	}

	line out = line();

	int op1_type = OPSTATE_NE, op2_type = OPSTATE_NE;
	try {
		out.op1 = get_op(strs[1], op1_type);
		if (num_of_items > 2) {
			out.op2 = get_op(strs[2], op2_type);
		}
		out.instr = get_instr(strs[0], op1_type, op2_type);
	}
	catch (err e) {
		throw err(e.msg + " in line: " + in);
	}

	return out;

}



//Is the string of the form <alphachars:>
bool isLabel(string s) {
	if (s.length() <= 2) return false;

	string name = s.substr(0, s.length() - 1);
	if (s[s.length() - 1] == ':') {
		if (isAlpha(name) && s.length() > 2) {
			return true;
		}
		else {
			throw err("Syntax error: invalid label name " + name);
		}
	}
	else {
		return false;
	}
}

void assemble(string input) {
	labels.clear();

	vector<string> str_lines_pre = split_str(input, '\n');
	int num_of_input_lines = str_lines_pre.size();

	//Remove empty lines and scan for labels
	vector<string> str_lines;
	int line_count = 0;
	for (int i = 0; i < num_of_input_lines; i++) {
		remove_comments(str_lines_pre[i]);

		if (str_lines_pre[i].length() != 0) {
			//Is it a label?
			if (isLabel(str_lines_pre[i])) {
				labels[str_lines_pre[i].substr(0, str_lines_pre[i].length() - 1)] = line_count;
				continue;
			}
			else {
				str_lines.push_back(str_lines_pre[i]);
				line_count++;
			}
		}


	}

	//Compile Lines
	for (int i = 0; i < line_count; i++) {
		PRGM[i] = parse_line(str_lines[i]);
	}
	PRGM_LEN = line_count;
}


int main(int argc, char* argv[])
{
	//Init
	init_maps();

	//Input filename?
	if (argc > 1) {
		ifstream srcf(argv[1]);

		//Is it valid?
		if (!srcf.is_open()) {
			cout << "Could not open file: " << argv[1] << endl << endl;
		}
		else {
			stringstream buf;
			buf << srcf.rdbuf();
			srcf.close();

			string src = buf.str();

			//Assmeble the file
			try {
				assemble(src);
			}
			catch (err e) {
				cout << e.msg << endl;
			}

			run();
			return 0;
		}



	}



	string filename;
	bool print_debug = false;

	do {
		cout << "filename: ";
		getline(cin, filename);

		if (filename == "debug true") {
			print_debug = true;
			cout << "debug: true" << endl << endl;
			continue;
		}
		else if (filename == "debug false") {
			print_debug = false;
			cout << "debug: false" << endl << endl;
			continue;
		}
		else if (filename == "exit") {
			return 1;
		}
		else {
			if (filename.find(".txt") == std::string::npos) {
				filename += ".txt";
			}

			//Read the file
			ifstream srcf(filename.c_str());

			//Is it valid?
			if (!srcf.is_open()) {
				cout << "Could not open file: " << filename << endl << endl;
				continue;
			}

			stringstream buf;
			buf << srcf.rdbuf();
			srcf.close();

			string src = buf.str();

			//Assmeble the file
			try {
				assemble(src);
			}
			catch (err e) {
				cout << e.msg << endl;
			}

			if (print_debug) {
				//For debugging
				for (int i = 0; i < PRGM_LEN; i++) {
					cout << "line " << i + 1 << ": " << PRGM[i].instr << " " << PRGM[i].op1 << " " << PRGM[i].op2 << endl;
				}
				cout << endl;
				for (std::map<string, int>::const_iterator it = labels.begin(); it != labels.end(); it++) {
					cout << it->first << " = line # " << it->second << endl;
				}
				cout << endl;

				cout << "Program output:" << endl;
			}

			//run
			run();
			cout << endl;
		}

	} while (1);


	return 0;
}
