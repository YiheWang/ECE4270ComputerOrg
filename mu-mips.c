#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "mu-mips.h"

/***************************************************************/
/* Print out a list of commands available                                                                  */
/***************************************************************/

int sign_extention(int bit)  //sign extention function
{
	if((bit & 0x00008000) == 0x00008000)
		bit |= 0xffff0000;   // negative, fill with 1
	else
		bit &= 0x0000ffff;   //positive fill with 0, keep latter 16 bits unchange
	return bit;
}

void help() {
	printf("------------------------------------------------------------------\n\n");
	printf("\t**********MU-MIPS Help MENU**********\n\n");
	printf("sim\t-- simulate program to completion \n");
	printf("run <n>\t-- simulate program for <n> instructions\n");
	printf("rdump\t-- dump register values\n");
	printf("reset\t-- clears all registers/memory and re-loads the program\n");
	printf("input <reg> <val>\t-- set GPR <reg> to <val>\n");
	printf("mdump <start> <stop>\t-- dump memory from <start> to <stop> address\n");
	printf("high <val>\t-- set the HI register to <val>\n");
	printf("low <val>\t-- set the LO register to <val>\n");
	printf("print\t-- print the program loaded into memory\n");
	printf("?\t-- display help menu\n");
	printf("quit\t-- exit the simulator\n\n");
	printf("------------------------------------------------------------------\n\n");
}

/***************************************************************/
/* Read a 32-bit word from memory                                                                            */
/***************************************************************/
uint32_t mem_read_32(uint32_t address)
{
	int i;
	for (i = 0; i < NUM_MEM_REGION; i++) {
		if ( (address >= MEM_REGIONS[i].begin) &&  ( address <= MEM_REGIONS[i].end) ) {
			uint32_t offset = address - MEM_REGIONS[i].begin;
			return (MEM_REGIONS[i].mem[offset+3] << 24) |
					(MEM_REGIONS[i].mem[offset+2] << 16) |
					(MEM_REGIONS[i].mem[offset+1] <<  8) |
					(MEM_REGIONS[i].mem[offset+0] <<  0);
		}
	}
	return 0;
}

/***************************************************************/
/* Write a 32-bit word to memory                                                                                */
/***************************************************************/
void mem_write_32(uint32_t address, uint32_t value)
{
	int i;
	uint32_t offset;
	for (i = 0; i < NUM_MEM_REGION; i++) {
		if ( (address >= MEM_REGIONS[i].begin) && (address <= MEM_REGIONS[i].end) ) {
			offset = address - MEM_REGIONS[i].begin;

			MEM_REGIONS[i].mem[offset+3] = (value >> 24) & 0xFF;
			MEM_REGIONS[i].mem[offset+2] = (value >> 16) & 0xFF;
			MEM_REGIONS[i].mem[offset+1] = (value >>  8) & 0xFF;
			MEM_REGIONS[i].mem[offset+0] = (value >>  0) & 0xFF;
		}
	}
}

/***************************************************************/
/* Execute one cycle                                                                                                              */
/***************************************************************/
void cycle() {
	handle_instruction();
	CURRENT_STATE = NEXT_STATE;
	INSTRUCTION_COUNT++;
}

/***************************************************************/
/* Simulate MIPS for n cycles                                                                                       */
/***************************************************************/
void run(int num_cycles) {

	if (RUN_FLAG == FALSE) {
		printf("Simulation Stopped\n\n");
		return;
	}

	printf("Running simulator for %d cycles...\n\n", num_cycles);
	int i;
	for (i = 0; i < num_cycles; i++) {
		if (RUN_FLAG == FALSE) {
			printf("Simulation Stopped.\n\n");
			break;
		}
		cycle();
	}
}

/***************************************************************/
/* simulate to completion                                                                                               */
/***************************************************************/
void runAll() {
	if (RUN_FLAG == FALSE) {
		printf("Simulation Stopped.\n\n");
		return;
	}

	printf("Simulation Started...\n\n");
	while (RUN_FLAG){
		cycle();
	}
	printf("Simulation Finished.\n\n");
}

/***************************************************************/
/* Dump a word-aligned region of memory to the terminal                              */
/***************************************************************/
void mdump(uint32_t start, uint32_t stop) {
	uint32_t address;

	printf("-------------------------------------------------------------\n");
	printf("Memory content [0x%08x..0x%08x] :\n", start, stop);
	printf("-------------------------------------------------------------\n");
	printf("\t[Address in Hex (Dec) ]\t[Value]\n");
	for (address = start; address <= stop; address += 4){
		printf("\t0x%08x (%d) :\t0x%08x\n", address, address, mem_read_32(address));
	}
	printf("\n");
}

/***************************************************************/
/* Dump current values of registers to the teminal                                              */
/***************************************************************/
void rdump() {
	int i;
	printf("-------------------------------------\n");
	printf("Dumping Register Content\n");
	printf("-------------------------------------\n");
	printf("# Instructions Executed\t: %u\n", INSTRUCTION_COUNT);
	printf("PC\t: 0x%08x\n", CURRENT_STATE.PC);
	printf("-------------------------------------\n");
	printf("[Register]\t[Value]\n");
	printf("-------------------------------------\n");
	for (i = 0; i < MIPS_REGS; i++){
		printf("[R%d]\t: 0x%08x\n", i, CURRENT_STATE.REGS[i]);
	}
	printf("-------------------------------------\n");
	printf("[HI]\t: 0x%08x\n", CURRENT_STATE.HI);
	printf("[LO]\t: 0x%08x\n", CURRENT_STATE.LO);
	printf("-------------------------------------\n");
}

/***************************************************************/
/* Read a command from standard input.                                                               */
/***************************************************************/
void handle_command() {
	char buffer[20];
	uint32_t start, stop, cycles;
	uint32_t register_no;
	int register_value;
	int hi_reg_value, lo_reg_value;

	printf("MU-MIPS SIM:> ");

	if (scanf("%s", buffer) == EOF){
		exit(0);
	}

	switch(buffer[0]) {
		case 'S':
		case 's':
			runAll();
			break;
		case 'M':
		case 'm':
			if (scanf("%x %x", &start, &stop) != 2){
				break;
			}
			mdump(start, stop);
			break;
		case '?':
			help();
			break;
		case 'Q':
		case 'q':
			printf("**************************\n");
			printf("Exiting MU-MIPS! Good Bye...\n");
			printf("**************************\n");
			exit(0);
		case 'R':
		case 'r':
			if (buffer[1] == 'd' || buffer[1] == 'D'){
				rdump();
			}else if(buffer[1] == 'e' || buffer[1] == 'E'){
				reset();
			}
			else {
				if (scanf("%d", &cycles) != 1) {
					break;
				}
				run(cycles);
			}
			break;
		case 'I':
		case 'i':
			if (scanf("%u %i", &register_no, &register_value) != 2){
				break;
			}
			CURRENT_STATE.REGS[register_no] = register_value;
			NEXT_STATE.REGS[register_no] = register_value;
			break;
		case 'H':
		case 'h':
			if (scanf("%i", &hi_reg_value) != 1){
				break;
			}
			CURRENT_STATE.HI = hi_reg_value;
			NEXT_STATE.HI = hi_reg_value;
			break;
		case 'L':
		case 'l':
			if (scanf("%i", &lo_reg_value) != 1){
				break;
			}
			CURRENT_STATE.LO = lo_reg_value;
			NEXT_STATE.LO = lo_reg_value;
			break;
		case 'P':
		case 'p':
			print_program();
			break;
		default:
			printf("Invalid Command.\n");
			break;
	}
}

/***************************************************************/
/* reset registers/memory and reload program                                                    */
/***************************************************************/
void reset() {
	int i;
	/*reset registers*/
	for (i = 0; i < MIPS_REGS; i++){
		CURRENT_STATE.REGS[i] = 0;
	}
	CURRENT_STATE.HI = 0;
	CURRENT_STATE.LO = 0;

	for (i = 0; i < NUM_MEM_REGION; i++) {
		uint32_t region_size = MEM_REGIONS[i].end - MEM_REGIONS[i].begin + 1;
		memset(MEM_REGIONS[i].mem, 0, region_size);
	}

	/*load program*/
	load_program();

	/*reset PC*/
	INSTRUCTION_COUNT = 0;
	CURRENT_STATE.PC =  MEM_TEXT_BEGIN;
	NEXT_STATE = CURRENT_STATE;
	RUN_FLAG = TRUE;
}

/***************************************************************/
/* Allocate and set memory to zero                                                                            */
/***************************************************************/
void init_memory() {
	int i;
	for (i = 0; i < NUM_MEM_REGION; i++) {
		uint32_t region_size = MEM_REGIONS[i].end - MEM_REGIONS[i].begin + 1;
		MEM_REGIONS[i].mem = malloc(region_size);
		memset(MEM_REGIONS[i].mem, 0, region_size);
	}
}

/**************************************************************/
/* load program into memory                                                                                      */
/**************************************************************/
void load_program() {
	FILE * fp;
	int i, word;
	uint32_t address;

	/* Open program file. */
	fp = fopen(prog_file, "r");
	if (fp == NULL) {
		printf("Error: Can't open program file %s\n", prog_file);
		exit(-1);
	}

	/* Read in the program. */

	i = 0;
	while( fscanf(fp, "%x\n", &word) != EOF ) {
		address = MEM_TEXT_BEGIN + i;
		mem_write_32(address, word);
		printf("writing 0x%08x into address 0x%08x (%d)\n", word, address, address);
		i += 4;
	}
	PROGRAM_SIZE = i/4;
	printf("Program loaded into memory.\n%d words written into memory.\n\n", PROGRAM_SIZE);
	fclose(fp);
}

/************************************************************/
/* decode and execute instruction                                                                     */
/************************************************************/
void handle_instruction()
{
	int instruction;
	instruction = mem_read_32(CURRENT_STATE.PC);
	int immediate;
	int offset;
	int rt;
	int rs;
	int rd;
	int base;
	int funct;
	int op;
	int64_t t1;
	uint64_t t2;
	int sa;
	int temp;
	uint32_t target1;
	

	op = instruction >> 26;
	if(op == 0x0){
		// Register is R-type
		rd = (instruction >> 11) & 0x1F; // bits 11 - 15
		rt = (instruction >> 16) & 0x1F; // bits 16 - 20
		rs = (instruction >> 21) & 0x1F; // bits 21 - 25
		funct = instruction & 0x3F;   //bits 0 - 5
		sa = (instruction >> 6) & 0x1F; //???

		switch(funct){
			case 0x20:
				//instruction ADD, bits 0 - 5: 10 0000
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
				printf("Instruction ADD!\n");
				break;

			case 0x21:
				//instruction ADDU, bits 0 - 5: 10 0001
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
				printf("Instruction ADDU!\n");
				break;

			case 0x24:
				//instruction AND, bits 0 - 5: 10 0100
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
				printf("Instruction AND!\n");
				break;

			case 0x25:
				//instruction OR, bits 0 - 5: 10 0101
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
				printf("Instruction OR!\n");
				break;

			case 0x26:
				//instruction XOR, bits 0 - 5: 10 0110
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt];
				printf("Instruction XOR!\n");
				break;

			case 0x27:
				//instruction NOR, bits 0 - 5: 10 0111
				NEXT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
				printf("Instruction NOR!\n");
				break;

			case 0x1A:
				//instruction DIV, bits 0 - 5: 01 1010
				NEXT_STATE.LO = CURRENT_STATE.REGS[rs] / CURRENT_STATE.REGS[rt]; // div
				NEXT_STATE.HI = CURRENT_STATE.REGS[rs] % CURRENT_STATE.REGS[rt]; // mod
				printf("Instruction DIV!\n");
				break;

			case 0x1B:
				//instruction DIVU, bits 0 - 5: 01 1011
				NEXT_STATE.LO = CURRENT_STATE.REGS[rs] / CURRENT_STATE.REGS[rt]; // div
				NEXT_STATE.HI = CURRENT_STATE.REGS[rs] % CURRENT_STATE.REGS[rt]; // mod
				printf("Instruction DIVU!\n");
				break;

			case 0x18:
				//instruction MULT, bits 0 - 5: 01 1000
				t1 = (int64_t)CURRENT_STATE.REGS[rs] * (int64_t)CURRENT_STATE.REGS[rt]; // multiple
				NEXT_STATE.LO = t1 & 0x0000FFFF;
				NEXT_STATE.HI = (t1 >> 32) & 0x0000FFFF;
				printf("Instruction MULT!\n");
				break;

			case 0x19:
				//instruction MULTU, bits 0 - 5: 01 1001
				t2 = (int64_t)CURRENT_STATE.REGS[rs] * (int64_t)CURRENT_STATE.REGS[rt]; // multiple unsigned
				NEXT_STATE.LO = t2 & 0x0000FFFF;
				NEXT_STATE.HI = (t2 >> 32) & 0x0000FFFF;
				printf("Instruction MULTU!\n");
				break;

			case 0x22:
				//instruction SUB, bits 0 - 5: 10 0010
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
				printf("Instruction SUB!\n");
				break;

			case 0x23:
				//instruction SUBU, bits 0 - 5: 10 0011
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
				printf("Instruction SUBU!\n");
				break;

			case 0x10:
				//instruction MFHI, bits 0 - 5: 01 0000
				NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
				printf("Instruction MFHI!\n");
				break;

			case 0x12:
				//instruction MFLO, bits 0 - 5: 01 0000
				NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
				printf("Instruction MFLO!\n");
				break;

			case 0x11:
				//instruction MTHI, bits 0 - 5: 01 0000
				NEXT_STATE.HI = CURRENT_STATE.REGS[rs];
				printf("Instruction MTHI!\n");
				break;

			case 0x13:
				//instruction MTLO, bits 0 - 5: 01 0000
				NEXT_STATE.LO = CURRENT_STATE.REGS[rs];
				printf("Instruction MTLO!\n");
				break;

			case 0x2A:
				//instruction SLT, bits 0 - 5: 10 1010
				if(CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt])
					NEXT_STATE.REGS[rd] = 0x00000001;
				else
					NEXT_STATE.REGS[rd] = 0x00000000;
				printf("Instruction SLT!\n");
				break;

			case 0x2:
 				// 31 - 26 000000  ; SRL instruction ; 0-5 000010 ; 2
				CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> sa;
				break;

			case 0x0:
				// 31 - 26 000000  ; SLL instruction ; 0-5 000000 ; 0
				CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rt] << sa;
				CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt];
				break;

			case 0x3:
				// 31 - 26 000000  ; SRA instruction ; 0-5 000011 ; 3
				temp = CURRENT_STATE.REGS[rt] << (32 - sa);
				temp = temp & 0xffffffff;
				CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rt] >> sa;
				CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] & temp;
				break;

			case 0x9: 
				//JALR 000011
				printf("JALR instruction\n");
				break;

			case 0x8: 
				//JR 001000
				printf("JR instruction\n");
				break;
			
		}
	}
	// for BGEZ  only .
	else if(op == 0x1){
		offset = instruction & 0xFFFF; // bits 0 - 15;
		funct = (instruction >> 16) & 0x1F; // bits 16 - 20
		rs = (instruction >> 21) & 0x1F; // bits 21 - 25
		// instruction BGEZ, bits 16 - 20: 00001
		uint32_t target = offset << 2;
		if(((instruction >> 15) & 0x1) == 0x1){
			target = target | 0xFFFC0000;
			// bits 2 - 17 unchanged, bits 16 - 31 are all 1
		}
		else if(((instruction >> 15) & 0x1) == 0x0){
			target = target & 0x00003FFC;
		}

		//bit 31 equal to 0
		if(((CURRENT_STATE.REGS[rs] & 0x80000000) == 0x0)){
			NEXT_STATE.PC = CURRENT_STATE.PC + target;
		}
		printf("Instruction BGEZ!\n");
	}
	// Register is J-type or I-type
	else {
		//immediate = (instruction << 16) & 0xFFFFFFFF;
		rd = (instruction >> 11) & 0x1F; // bits 11 - 15
		immediate = instruction & 0xFFFF; // bits 0 - 15;
		offset = instruction & 0xFFFF; // bits 0 - 15;
		rt = (instruction >> 16) & 0x1F; // bits 16 - 20
		rs = (instruction >> 21) & 0x1F; // bits 21 - 25
		base = (instruction >> 21) & 0x1F; // bits 21 - 25
		int target = instruction & 0x03FFFFFF; // bits 0 - 25
		switch(op){
			case 0x08:
			//instruction ADDI, bits 26 - 31: 00 1000
				if(((instruction >> 15) & 0x1) == 0x1){
					immediate = immediate | 0xFFFF0000;
					// bits 0 - 15 unchanged, bits 16 - 31 are all 1
					NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + immediate;
				}
				else if(((instruction >> 15) & 0x1) == 0x0){
					immediate = immediate & 0x0000FFFF;
					NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + immediate;
				}
				printf("Instruction ADDI!\n");
				break;
			case 0x09:
			// instrucntion ADDIU, bits 26 - 31: 00 1001
				if(((instruction >> 15) & 0x1) == 0x1){
					immediate = immediate | 0xFFFF0000;
					// bits 0 - 15 unchanged, bits 16 - 31 are all 1
					NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + immediate;
				}
				else if(((instruction >> 15) & 0x1) == 0x0){
					immediate = immediate & 0x0000FFFF;
					NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + immediate;
				}
				printf("Instruction ADDIU!\n");
				break;
			case 0xA: 
				//SLTI 001010
				immediate = sign_extention(immediate);
				if(CURRENT_STATE.REGS[rs] < immediate){
					NEXT_STATE.REGS[rd] = 0x00000001;
				}
				else{
					NEXT_STATE.REGS[rd] = 0x00000000;
				}
				break;
			case 0xC:
			// instrucntion ANDI, bits 26 - 31: 00 1100
				temp = instruction & 0xFFFF;
				CURRENT_STATE.REGS[rs] = CURRENT_STATE.REGS[rs] & 0xFFFF;
				NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & temp;
				printf("Instruction ANDI!\n");
				break;
			case 0xF:
			// instrucntion LUI, bits 26 - 31: 00 1111
				immediate  = (instruction << 16) & 0xFFFFFFFF;
				NEXT_STATE.REGS[rt] = immediate;
				printf("Instruction LUI!\n");
				break;
			case 0x20:
			//LB 100000
				offset = sign_extention(offset);
				//temp = ((offset << 16) + offset) & 0xffffffff;
				//contents of the byte at memory location
				mem_write_32(CURRENT_STATE.REGS[base] + offset, CURRENT_STATE.REGS[rt] & 0x000000FF);
				printf("Instruction LB!\n");
				break;
			case 0x21:
			//LH 100001
				offset = sign_extention(offset);
				//temp = ((offset << 16) + offset) & 0xffffffff;
				mem_write_32(CURRENT_STATE.REGS[base] + offset, CURRENT_STATE.REGS[rt]);
				printf("Instruction LH!\n");
				break;
			case 0x2B:
			//SW 101011
				/*offset = sign_extention(offset);
				mem_write_32(CURRENT_STATE.REGS[base] + offset, CURRENT_STATE.REGS[rt]);*/
				printf("Instruction SW!\n");
				break;
			 case 0x29:
			 //SH
				offset = sign_extention(offset);
				//the least half word store
				mem_write_32(CURRENT_STATE.REGS[base] + offset, CURRENT_STATE.REGS[rt] & 0x0000ffff);
				printf("Instruction SH!\n");
				break;
			// may have issue
			case 0x04:
			// instrucntion BEQ, bits 26 - 31: 00 0100
				target1 = offset << 2;
				if(((instruction >> 15) & 0x1) == 0x1){
					target1 = target1 | 0xFFFC0000;
					// bits 2 - 17 unchanged, bits 18 - 31 are all 1
				}
				else if(((instruction >> 15) & 0x1) == 0x0){
					target1 = target1 & 0x00003FFC;
				}
				if(CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt]){
					NEXT_STATE.PC = CURRENT_STATE.PC + target;
				}
				printf("Instruction BEQ!\n");
				break;
			case 0x05:
			// instrucntion BNE, bits 26 - 31: 00 0101
				target1 = offset << 2;
				if(((instruction >> 15) & 0x1) == 0x1){
					target1 = target1 | 0xFFFC0000;
					// bits 2 - 17 unchanged, bits 18 - 31 are all 1
				}
				else if(((instruction >> 15) & 0x1) == 0x0){
					target1 = target1 & 0x00003FFC;
				}
				if(CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt]){
					NEXT_STATE.PC = CURRENT_STATE.PC + target;
				}
				printf("Instruction BNE!\n");
				break;
			case 0x06:
			// instrucntion BLEZ, bits 26 - 31: 00 0110
				target1 = offset << 2;
				if(((instruction >> 15) & 0x1) == 0x1){
					target = target | 0xFFFC0000;
					// bits 2 - 17 unchanged, bits 18 - 31 are all 1
				}
				else if(((instruction >> 15) & 0x1) == 0x0){
					target = target & 0x00003FFC;
				}
				if(((CURRENT_STATE.REGS[rs] & 0x80000000) == 0x80000000) || (CURRENT_STATE.REGS[rs] == 0x0)){
					NEXT_STATE.PC = CURRENT_STATE.PC + target;
				}
				printf("Instruction BLEZ!\n");
				break;
			case 0x01:
			// instrucntion BLTZ, bits 26 - 31: 00 0001
				target = offset << 2;
				if(((instruction >> 15) & 0x1) == 0x1){
					target = target | 0xFFFC0000;
					// bits 2 - 17 unchanged, bits 16 - 31 are all 1
				}
				else if(((instruction >> 15) & 0x1) == 0x0){
					target = target & 0x00003FFC;
				}
				if(((CURRENT_STATE.REGS[rs] & 0x80000000) == 0x80000000)){
					NEXT_STATE.PC = CURRENT_STATE.PC + target;
				}
				printf("Instruction BLTZ!\n");
				break;
			case 0x07:
			// instrucntion BGTZ, bits 26 - 31: 00 0111
				target = offset << 2;
				if(((instruction >> 15) & 0x1) == 0x1){
					target = target | 0xFFFC0000;
					// bits 2 - 17 unchanged, bits 16 - 31 are all 1
				}
				else if(((instruction >> 15) & 0x1) == 0x0){
					target = target & 0x00003FFC;
				}
				//bit 31 equal to 0  and other bits not all equal to 0
				if(((CURRENT_STATE.REGS[rs] & 0x80000000) == 0x0) && (CURRENT_STATE.REGS[rs] != 0x0)){
					NEXT_STATE.PC = CURRENT_STATE.PC + target;
				}
				printf("Instruction BGTZ!\n");
				break;
			case 0x2: 
			//J jump 000010  ----------------------
				target = target << 2;
				NEXT_STATE.PC = (NEXT_STATE.PC & 0xf0000000) + target;
				printf("Instruction J!\n");
				break;
			case 0x3: 
			//JAL 000011
				printf("JAL instruction \n");
				break;
		}
	}
 	NEXT_STATE.PC = CURRENT_STATE.PC + 4;
	/*IMPLEMENT THIS*/
	/* execute one instruction at a time. Use/update CURRENT_STATE and and NEXT_STATE, as necessary.*/
}
/************************************************************/
/* Initialize Memory                                                                                                    */
/************************************************************/
void initialize() {
	init_memory();
	CURRENT_STATE.PC = MEM_TEXT_BEGIN;
	NEXT_STATE = CURRENT_STATE;
	RUN_FLAG = TRUE;
}
/************************************************************/
/* Print the program loaded into memory (in MIPS assembly format)    */
/************************************************************/
void print_program(){
	int i;
	uint32_t addr;
	for(i=0; i<PROGRAM_SIZE; i++){
		addr = MEM_TEXT_BEGIN + (i*4);
		printf("[0x%x]\t", addr);
		print_instruction(addr);
	}
}
/************************************************************/
/* Print the instruction at given memory address (in MIPS assembly format)    */
/************************************************************/
void print_instruction(uint32_t addr){
	/*IMPLEMENT THIS*/
	int instruction = mem_read_32(addr);
	int immediate;
	int offset;
	int rt;
	int rs;
	int rd;
	int base;
	int funct;
	int op;
	int sa = (instruction >> 6) & 0x1F; //???
	int target = instruction & 0x03FFFFFF;
	
	op = instruction >> 26;
	if(op == 0x0){
		// Register is R-type
		rd = (instruction >> 11) & 0x1F; // bits 11 - 15
		rt = (instruction >> 16) & 0x1F; // bits 16 - 20
		rs = (instruction >> 21) & 0x1F; // bits 21 - 25
		funct = instruction & 0x3F;   //bits 0 - 5
		switch(funct){
			case 0x20:
				//printf("Instruction ADD!\n");
				printf("ADD $%d, $%d, $%d \n", rd, rs, rt);
				break;
			case 0x21:
				//printf("Instruction ADDU!\n");
				printf("ADDU $%d, $%d, $%d \n", rd, rs, rt);
				break;
			case 0x24:
				//instruction AND, bits 0 - 5: 10 0100
				//printf("Instruction AND!\n");
				printf("AND $%d, $%d, $%d \n", rd, rs, rt);
				break;
			case 0x25:
				//instruction OR, bits 0 - 5: 10 0101
				//printf("Instruction OR!\n");
				printf("OR $%d, $%d, $%d \n", rd, rs, rt);
				break;
			case 0x26:
				//instruction XOR, bits 0 - 5: 10 0110
				//printf("Instruction XOR!\n");
				printf("XOR $%d, $%d, $%d \n", rd, rs, rt);
				break;
			case 0x27:
				//instruction NOR, bits 0 - 5: 10 0111
				//printf("Instruction NOR!\n");
				printf("NOR $%d, $%d, $%d \n", rd, rs, rt);
				break;
			case 0x1A:
				//instruction DIV, bits 0 - 5: 01 1010
				printf("DIV $%d, $%d \n",rs, rt);
				//printf("Instruction DIV!\n");
				break;
			case 0x1B:
				//instruction DIVU, bits 0 - 5: 01 1011
				printf("DIVU $%d, $%d \n",rs, rt);
				//printf("Instruction DIVU!\n");
				break;
			case 0x18:
				//instruction MULT, bits 0 - 5: 01 1000
				//printf("Instruction MULT!\n");
				printf("MULT $%d, $%d \n", rs, rt);
				break;
			case 0x19:
				//instruction MULTU, bits 0 - 5: 01 1001
				//printf("Instruction MULTU!\n");
				printf("MULTU $%d, $%d \n", rs, rt);
				break;
			case 0x22:
				//instruction SUB, bits 0 - 5: 10 0010
				//printf("Instruction SUB!\n");
				printf("SUB $%d, $%d, $%d \n", rd, rs, rt);
				break;
			case 0x23:
				//instruction SUBU, bits 0 - 5: 10 0011
				//printf("Instruction SUBU!\n");
				printf("SUBU $%d, $%d, $%d \n", rd, rs, rt);
				break;
			case 0x10:
				//instruction MFHI, bits 0 - 5: 01 0000
				//printf("Instruction MFHI!\n");
				printf("MFHI $%d\n", rd);
				break;
			case 0x12:
				//instruction MFLO, bits 0 - 5: 01 0000
				//printf("Instruction MFLO!\n");
				printf("MFLO $%d\n", rd);
				break;
			case 0x11:
				//instruction MTHI, bits 0 - 5: 01 0000
				//printf("Instruction MTHI!\n");
				printf("MTHI $%d\n", rs);
				break;
			case 0x13:
				//instruction MTLO, bits 0 - 5: 01 0000
				//printf("Instruction MTLO!\n");
				printf("MTLO $%d\n", rs);
				break;
			case 0x2A:
				//instruction SLT, bits 0 - 5: 10 1010
				//printf("Instruction SLT!\n");
				printf("SLT $%d, $%d, $%d\n", rd, rs, rt);
				break;
			case 0x2:
	 			// 31 - 26 000000  ; SRL instruction ; 0-5 000010 ; 2
				printf("SRL $%d, $%d, $%d \n", rd, rt, sa);
				break;
			case 0x0:
				// 31 - 26 000000  ; SLL instruction ; 0-5 000000 ; 0
 				printf("SLL $%d, $%d, $%d \n", rd, rt, sa);
				break;
			case 0x3:
				// 31 - 26 000000  ; SRA instruction ; 0-5 000011 ; 3
				printf("SRA $%d, $%d, $%d \n", rd, rt, sa);
				break;
			case 0x9: //JALR 001001
				printf("JALR $%d ", rs);
				printf("JALR $%d, $%d \n", rd, rs);
				break;
			case 0x8: //JR 001000
				printf("JR $%d \n", rs);
				break;
		}
	}
	// for BGEZ  only .
	else if(op == 0x1){
		offset = instruction & 0xFFFF; // bits 0 - 15;
		funct = (instruction >> 16) & 0x1F; // bits 16 - 20
		rs = (instruction >> 21) & 0x1F; // bits 21 - 25
		
		printf("BGEZ $%d, %d\n",rs, offset);
	}
	// Register is J-type or I-type
	else {
		//immediate = (instruction << 16) & 0xFFFFFFFF;
		immediate = instruction & 0xFFFF; // bits 0 - 15;
		offset = instruction & 0xFFFF; // bits 0 - 15;
		rt = (instruction >> 16) & 0x1F; // bits 16 - 20
		rs = (instruction >> 21) & 0x1F; // bits 21 - 25
		base = (instruction >> 21) & 0x1F; // bits 21 - 25
		switch(op){
			case 0x08:
			//instruction ADDI, bits 26 - 31: 00 1000
				//printf("Instruction ADDI!\n");
				printf("ADDI $%d, $%d, %d \n", rt, rs, immediate);
				break;
			case 0xC:   // 001100 ANDI instruction
				printf("ANDI $%d, $%d, %d \n", rt, rs, immediate);
				break;
			case 0xD: // ORI instruction 001101
				printf("ORI $%d, $%d, %d \n", rt, rs, immediate);
				break;
			case 0xE: 
			//XORI Instruction; 001110;
				printf("XORI $%d, $%d, %d \n", rt, rs, immediate);
				break;
			case 0x09:
			// instrucntion ADDIU, bits 26 - 31: 00 1001
				//printf("Instruction ADDIU!\n");
				printf("ADDIU $%d, $%d, %d \n", rt, rs, immediate);
				break;
			case 0xF:
			// instrucntion LUI, bits 26 - 31: 00 1111
				//printf("Instruction LUI!\n");
				printf("LUI $%d, %d \n", rt, immediate);
				break;
			case 0x20:
			//LB 100000
				//printf("Instruction LB!\n");
				printf("LB $%d, $%d(%d) \n", rt, offset, base);
				break;
			case 0x21:
			//LH 100001
				//printf("Instruction LH!\n");
				printf("LH $%d, $%d(%d) \n", rt, offset, base);
				break;
			case 0x23:
			//SW 101011
				//printf("Instruction SW!\n");
				printf("SW $%d, $%d(%d) \n", rt, offset, base);
				break;
			case 0x29:
			//SH
				//printf("Instruction SH!\n");
				printf("SH $%d, $%d(%d) \n", rt, offset, base);
				break;
			case 0x28:
 			//SB
 				//printf("Instruction SB!\n");
				printf("SB $%d, $%d(%d) \n", rt, offset, base);
 				break;
			case 0x0A:
 			//SLTI
 				//printf("Instruction SB!\n");
				printf("SLTI $%d, $%d, %d \n", rt, rs, immediate);
 				break;
			case 0x04:
			// instrucntion BEQ, bits 26 - 31: 00 0100
				//printf("Instruction BEQ!\n");
				printf("BEQ $%d, $%d, %d \n", rs, rt, offset);
				break;
			case 0x05:
			// instrucntion BNE, bits 26 - 31: 00 0101
				//printf("Instruction BNE!\n");
				printf("BNE $%d, $%d, %d \n", rs, rt, offset);
				break;
			case 0x06:
			// instrucntion BLEZ, bits 26 - 31: 00 0110
				//printf("Instruction BLEZ!\n");
				printf("BLEZ $%d, %d \n", rs, offset);
				break;
			case 0x01:
			// instrucntion BLTZ, bits 26 - 31: 00 0001
				//printf("Instruction BLTZ!\n");
				printf("BLTZ $%d, %d \n", rs, offset);
				break;
			case 0x07:
			// instrucntion BGTZ, bits 26 - 31: 00 0111
				//printf("Instruction BGTZ!\n");
				printf("BGTZ $%d, %d \n", rs, offset);
				break;
			case 0x2: //J jump 000010  ----------------------
				//printf("Instruction J!\n");
				printf("J %d \n", target);
				break;
			case 0x3: //JAL 000011
				printf("JAL %d \n", target);
				break;
		}
	}
		NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
/***************************************************************/
/* main                                                                                                                                   */
/***************************************************************/
int main(int argc, char *argv[]) {
	printf("\n**************************\n");
	printf("Welcome to MU-MIPS SIM...\n");
	printf("**************************\n\n");
	if (argc < 2) {
		printf("Error: You should provide input file.\nUsage: %s <input program> \n\n",  argv[0]);
		exit(1);
	}
	strcpy(prog_file, argv[1]);
	initialize();
	load_program();
	help();
	while (1){
		handle_command();
	}
	return 0;
}
