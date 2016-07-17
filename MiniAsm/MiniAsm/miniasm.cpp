#pragma once

#include <string>
#include <iostream>

#include "miniasm.h"

namespace miniasm
{

	int REG[REG_SIZE]; //A-H
	int RAM[RAM_SIZE];
	line PRGM[MAX_LINES];

	const int LD_AA = 1; //ld adr adr
	const int LD_AL = 2; //ld adr num
	const int LD_DA = 3; //ld &reg adr
	const int LD_DR = 4; //ld &reg reg
	const int LD_RL = 5; //ld reg num
	const int LD_RA = 6; //ld reg adr
	const int LD_RD = 7; //ld reg &reg
	const int LD_RR = 8; //ld reg reg


	const int PRT = 11;
	const int PRTCH = 12;
	const int JNZ = 13;
	const int JP = 14;
	const int EXIT = 15;

	//Arithmatic
	const int ADD = 20;
	const int SUB = 21;
	const int MUL = 22;
	const int DIV = 23;
	const int REM = 24;
	const int INC = 25;
	const int DEC = 26;
	const int AND = 27;
	const int OR = 28;


	int PRGM_LEN; //Program Length

	//Get value from ram at address i
	int ram(int i) {
		if (i >= RAM_SIZE || i < 0) {
			throw err("Cannot access RAM at location " + i);
		}
		else {
			return RAM[i];
		}
	}

	//Set value in ram at address i to v
	void ramp(int i, int v) {
		if (i >= RAM_SIZE || i < 0) {
			throw err("Cannot access RAM at location " + i);
		}
		else {
			RAM[i] = v;
		}
	}

	//Get value from the ith register
	int reg(int i) {
		if (i >= REG_SIZE || i < 0) {
			throw err("Register does not exist: " + i);
		}
		else {
			return REG[i];
		}
	}

	//Set the value of the ith register to v
	void regp(int i, int v) {
		if (i >= REG_SIZE || i < 0) {
			throw err("Register does not exist: " + i);
		}
		else {
			REG[i] = v;
		}
	}

//Current Line
#define CL PRGM[lc]

	//Run the program
	int run() {
		int lc = 0; // Line counter
		while (lc < PRGM_LEN) {
			switch (CL.instr) {
			case LD_AA:
				//Load the int stored at location op2 into the location op1
				ramp(CL.op1, ram(CL.op2)); break;
			case LD_AL:
				//Load the value of the second op into the location op1
				ramp(CL.op1, CL.op2); break;
			case LD_RL:
				//Load the value of the second op into the register op1
				regp(CL.op1, CL.op2); break;
			case LD_RA:
				regp(CL.op1, ram(CL.op2));
			case LD_DA:
				//Load the int stored at location op2 into the
				//address stored at location op1
				//ld(deref(CL.op1), deref(CL.op2)); break;
				ramp(reg(CL.op1), ram(CL.op2)); break;
			case LD_DR:
				ramp(reg(CL.op1), reg(CL.op2)); break;
			case LD_RD:
				//Load the value stored in ram at the address stored
				//in the location in register op2
				regp(CL.op1, ram(reg(CL.op2)));	break;
			case LD_RR:
				//Load the value stored in ram at the address stored
				//in the location in register op2
				regp(CL.op1, reg(CL.op2));	break;
			case PRT:
				std::cout << reg(CL.op1); break;
			case PRTCH:
				//Print the value as a char
				std::cout << (char)(reg(CL.op1)); break;
			case JP:
				lc = CL.op1 - 1; break;
			case JNZ:
				//Jump if Not Zero
				if (reg(CL.op1) != 0) lc = CL.op2 - 1;
				break;
			case ADD:
				//Add the value from reg2 to reg1
				regp(CL.op1, reg(CL.op1) + reg(CL.op2)); break;
			case SUB:
				//Add the value from reg2 to reg1
				regp(CL.op1, reg(CL.op1) - reg(CL.op2)); break;
			case MUL:
				//Add the value from reg2 to reg1
				regp(CL.op1, reg(CL.op1) * reg(CL.op2)); break;
			case DIV:
				//Add the value from reg2 to reg1
				regp(CL.op1, reg(CL.op1) / reg(CL.op2)); break;
			case REM:
				//Add the value from reg2 to reg1
				regp(CL.op1, reg(CL.op1) % reg(CL.op2)); break;
			case AND:
				//bitwise and
				regp(CL.op1, reg(CL.op1) & reg(CL.op2)); break;
			case OR:
				//bitwise and
				regp(CL.op1, reg(CL.op1) | reg(CL.op2)); break;
			case INC:
				regp(CL.op1, reg(CL.op1) + 1); break;
			case DEC:
				regp(CL.op1, reg(CL.op1) - 1); break;
			case EXIT:
				std::cout << std::endl << "program exited with code " << CL.op1 << std::endl;
				return 1; //Success
			default:
				return 0; //Error
			}
			lc++;
		}
		std::cout << std::endl << "program complete" << std::endl;
		return 1; //Success
	}

};

