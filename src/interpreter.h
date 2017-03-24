#pragma once



namespace miniasm {

	const int REG_SIZE = 8;
	const int RAM_SIZE = 1024;
	const int MAX_LINES = 1024;

	struct line {
		line(int a, int b, int c) :instr(a), op1(b), op2(c) {}
		line() : instr(0), op1(0), op2(0) {}
		int instr;
		int op1;
		int op2;
	};

	struct err{
		err(std::string m) : msg(m) {}
		std::string msg;
	};

	extern const int LD_AA;
	extern const int LD_AL;
	extern const int LD_DA;
	extern const int LD_DR;
	extern const int LD_RL;
	extern const int LD_RA;
	extern const int LD_RD;
	extern const int LD_RR;

	extern const int PRT;
	extern const int PRTCH;
	extern const int JNZ;
	extern const int JP;
	extern const int EXIT;
	extern const int GETNUM;
	extern const int GETCH;
	extern const int SLP;

	//Arithmatic
	extern const int ADD;
	extern const int SUB;
	extern const int MUL;
	extern const int DIV;
	extern const int REM;
	extern const int INC;
	extern const int DEC;
	extern const int AND;
	extern const int OR;


	extern int REG[]; //A-H
	extern int RAM[];
	extern line PRGM[];

	extern int PRGM_LEN;

	int deref(int addr);
	void ld(int addr, int val);
	int run();
}
