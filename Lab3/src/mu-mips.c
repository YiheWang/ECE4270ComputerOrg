#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "mu-mips.h"

/***************************************************************/
/* Print out a list of commands available                                                                  */
/***************************************************************/
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
	printf("show\t-- print the current content of the pipeline registers\n");
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
			printf()
		}
	}
}

/***************************************************************/
/* Execute one cycle                                                                                                              */
/***************************************************************/
void cycle() {
	handle_pipeline();
	CURRENT_STATE = NEXT_STATE;
	CYCLE_COUNT++;
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
	printf("# Cycles Executed\t: %u\n", CYCLE_COUNT);
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
			if (buffer[1] == 'h' || buffer[1] == 'H'){
				show_pipeline();
			}else {
				runAll();
			}
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
/* maintain the pipeline                                                                                           */
/************************************************************/
void handle_pipeline()
{
	/*INSTRUCTION_COUNT should be incremented when instruction is done*/
	/*Since we do not have branch/jump instructions, INSTRUCTION_COUNT should be incremented in WB stage */

	WB();
	MEM();
	EX();
	ID();
	IF();
}

/************************************************************/
/* writeback (WB) pipeline stage:                                                                          */
/************************************************************/
void WB()
{
	/*IMPLEMENT THIS*/
	INSTRUCTION_COUNT++;
	uint32_t opcode;
	uint32_t funct;
	uint32_t rd;
	uint32_t rt;
	opcode = (MEM_WB.IR & 0xFC000000) >> 26;
	funct = MEM_WB.IR & 0x0000003F;
	rd = (MEM_WB.IR & 0x0000F800) >> 11;
	rt = (MEM_WB.IR & 0x001F0000) >> 16;

	//Different operation according to different instruction
	if(opcode == 0x00){
		switch(funct){
			case 0x00: //SLL, ALU Instruction
				NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
				break;
			case 0x02: //SRL, ALU Instruction
				NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
				break;
			case 0x03: //SRA, ALU Instruction
				NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
				break;
			/*case 0x0C: //SYSCALL

				break;*/
			case 0x10: //MFHI, Load/Store Instruction
				NEXT_STATE.REGS[rd] = MEM_WB.HI;
				break;
			case 0x11: //MTHI, Load/Store Instruction
				NEXT_STATE.HI = MEM_WB.ALUOutput;
				break;
			case 0x12: //MFLO, Load/Store Instruction
				NEXT_STATE.REGS[rd] = MEM_WB.LO;
				break;
			case 0x13: //MTLO, Load/Store Instruction
				NEXT_STATE.LO = MEM_WB.ALUOutput;
				break;
			case 0x18: //MULT, ALU Instruction
				NEXT_STATE.LO = MEM_WB.LO;
				NEXT_STATE.HI = MEM_WB.HI;
				break;
			case 0x19: //MULTU, ALU Instruction
				NEXT_STATE.LO = MEM_WB.LO;
				NEXT_STATE.HI = MEM_WB.HI;
				break;
			case 0x1A: //DIV, ALU Instruction
				NEXT_STATE.LO = MEM_WB.LO;
				NEXT_STATE.HI = MEM_WB.HI;
				break;
			case 0x1B: //DIVU, ALU Instruction
				NEXT_STATE.LO = MEM_WB.LO;
				NEXT_STATE.HI = MEM_WB.HI;
				break;
			case 0x20: //ADD, ALU Instruction
				NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
				break;
			case 0x21: //ADDU, ALU Instruction
				NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
				break;
			case 0x22: //SUB, ALU Instruction
				NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
				break;
			case 0x23: //SUBU, ALU Instruction
				NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
				break;
			case 0x24: //AND, ALU Instruction
				NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
				break;
			case 0x25: //OR, ALU Instruction
				NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
				break;
			case 0x26: //XOR, ALU Instruction
				NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
				break;
			case 0x27: //NOR, ALU Instruction
				NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
				break;
			case 0x2A: //SLT, ALU Instruction
				NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
				break;
			default:
				printf("Instruction at 0x%x is not implemented!\n", CURRENT_STATE.PC);
				break;
		}
	}
	else{
		switch(opcode){
			case 0x08: //ADDI, ALU Instruction
				NEXT_STATE.REGS[rt] = MEM_WB.ALUOutput;
				break;
			case 0x09: //ADDIU, ALU Instruction
				NEXT_STATE.REGS[rt] = MEM_WB.ALUOutput;
				break;
			case 0x0A: //SLTI, ALU Instruction
				NEXT_STATE.REGS[rt] = MEM_WB.ALUOutput;
				break;
			case 0x0C: //ANDI, ALU Instruction
				NEXT_STATE.REGS[rt] = MEM_WB.ALUOutput;
				break;
			case 0x0D: //ORI, ALU Instruction
				NEXT_STATE.REGS[rt] = MEM_WB.ALUOutput;
				break;
			case 0x0E: //XORI, ALU Instruction
				NEXT_STATE.REGS[rt] = MEM_WB.ALUOutput;
				break;
			case 0x0F: //LUI, Load/Store Instruction
				NEXT_STATE.REGS[rt] = MEM_WB.LMD;
				break;
			case 0x20: //LB, Load/Store Instruction
				NEXT_STATE.REGS[rt] = MEM_WB.LMD;
				break;
			case 0x21: //LH, Load/Store InstructionEM stage
ALU calculated insided stage 14 
ALU output insided stage 8 

				NEXT_STATE.REGS[rt] = MEM_WB.LMD;
				break;
			case 0x23: //LW, Load/Store Instruction
				NEXT_STATE.REGS[rt] = MEM_WB.LMD;
				break;
			case 0x28: //SB, Load/Store Instruction
				// do nothing
				break;
			case 0x29: //SH, Load/Store Instruction
				// do nothing
				break;
			case 0x2B: //SW, Load/Store Instruction
				// do nothing
				break;
			default:
				// put more things here
				printf("Instruction at 0x%x is not implemented!\n", CURRENT_STATE.PC);
				break;
		}
	}
}

/************************************************************/
/* memory access (MEM) pipeline stage:                                                          */
/************************************************************/
void MEM()
{
	/*IMPLEMENT THIS*/
	uint32_t opcode;
	uint32_t funct;
	uint32_t data;
	opcode = (EX_MEM.IR & 0xFC000000) >> 26;
	opcode = opcode & 0x000000CF;
	funct = EX_MEM.IR & 0x0000003F;

	MEM_WB.IR = EX_MEM.IR;
	//MEM_WB.LO = EX_MEM.LO;
	//MEM_WB.HI = EX_MEM.HI;
	//Different operation according to different instruction
	printf("MEM %d\n",opcode);
	if(opcode == 0x00){
		switch(funct){
			case 0x00: //SLL, ALU Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x02: //SRL, ALU Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x03: //SRA, ALU Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			/*case 0x0C: //SYSCALL

				break;*/
			case 0x10: //MFHI, Load/Store Instruction
				MEM_WB.HI = EX_MEM.HI;
				break;
			case 0x11: //MTHI, Load/Store Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x12: //MFLO, Load/Store Instruction
				MEM_WB.LO = EX_MEM.LO;
				break;
			case 0x13: //MTLO, Load/Store Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x18: //MULT, ALU Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x19: //MULTU, ALU Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x1A: //DIV, ALU Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x1B: //DIVU, ALU Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x20: //ADD, ALU Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x21: //ADDU, ALU Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x22: //SUB, ALU Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x23: //SUBU, ALU Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x24: //AND, ALU Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x25: //OR, ALU Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x26: //XOR, ALU Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x27: //NOR, ALU Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x2A: //SLT, ALU Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;imm
			default:
				printf("Instruction at 0x%x is not implemented!\n", CURRENT_STATE.PC);
				break;
		}
	}
	else{			printf(" opcode %d inside else\n",opcode);
		switch(opcode){

			case 0x08: //ADDI, ALU Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x09: //ADDIU, ALU Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x0A: //SLTI, ALU Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x0C: //ANDI, ALU Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x0D: //ORI, ALU Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x0E: //XORI, ALU Instruction
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x0F: //LUI, Load/Store Instruction
				MEM_WB.LMD = mem_read_32(EX_MEM.ALUOutput);
				break;
			case 0x20: //LB, Load/Store Instruction
				data = mem_read_32(EX_MEM.ALUOutput);
				MEM_WB.LMD = ((data & 0x000000FF) & 0x80) > 0 ? (data | 0xFFFFFF00) : (data & 0x000000FF);
				break;
			case 0x21: //LH, Load/Store Instruction
				data = mem_read_32(EX_MEM.ALUOutput);
				MEM_WB.LMD = ((data & 0x0000FFFF) & 0x8000) > 0 ? (data | 0xFFFF0000) : (data & 0x0000FFFF);
				break;
			case 0x23: //LW, Load/Store Instruction
				MEM_WB.LMD = mem_read_32(EX_MEM.ALUOutput);
				break;
			case 0x28: //SB, Load/Store Instruction
				data = mem_read_32(EX_MEM.ALUOutput);
				data = (data & 0xFFFFFF00) | (EX_MEM.B & 0x000000FF);
				mem_write_32(EX_MEM.ALUOutput, data);
				break;
			case 0x29: //SH, Load/Store Instruction
				data = mem_read_32(EX_MEM.ALUOutput);
				data = (data & 0xFFFF0000) | (EX_MEM.B & 0x0000FFFF);
				mem_write_32(EX_MEM.ALUOutput, data);
				break;
			case 0x2B: //SW, Load/Store Instruction
				printf("This is SW instruction\n");
				mem_write_32(EX_MEM.ALUOutput, EX_MEM.B);
				break;
			default:
				// put more things here
				printf("Instruction at 0x%x is not implemented!\n", CURRENT_STATE.PC);
				break;
		}
	}
	printf("MEM stage\n");
}

/************************************************************/
/* execution (EX) pipeline stage:                                                                          */
/************************************************************/
void EX()
{
	/*IMPLEMENT THIS*/
	uint32_t opcode;
	uint32_t funct;
	uint32_t sa;
	uint64_t product, p1, p2;
	sa = (ID_EX.IR & 0x000007C0) >> 6;
	opcode = (ID_EX.IR & 0xFC000000) >> 26;
	funct = ID_EX.IR & 0x0000003F;

	EX_MEM.IR = ID_EX.IR;
	EX_MEM.B = ID_EX.B;
	//Different operation according to different instruction
	if(opcode == 0x00){
		switch(funct){
			case 0x00: //SLL, ALU Instruction
				EX_MEM.ALUOutput = ID_EX.B << sa;
				break;
			case 0x02: //SRL, ALU Instruction
				EX_MEM.ALUOutput = ID_EX.B >> sa;
				break;
			case 0x03: //SRA, ALU Instruction
				if ((ID_EX.B & 0x80000000) == 1){
					EX_MEM.ALUOutput =  ~(~ID_EX.B >> sa );
				}
				else{
					EX_MEM.ALUOutput = ID_EX.B >> sa;
				}
				break;
			/*case 0x0C: //SYSCALL
				if(CURRENT_STATE.REGS[2] == 0xa){
					RUN_FLAG = FALSE;
				}
				break;*/
			case 0x10: //MFHI, Load/Store Instruction
				EX_MEM.HI = ID_EX.HI;
				break;
			case 0x11: //MTHI, Load/Store Instruction
				EX_MEM.ALUOutput = ID_EX.A;
				break;
			case 0x12: //MFLO, Load/Store Instruction
				EX_MEM.LO = ID_EX.LO;
				break;
			case 0x13: //MTLO, Load/Store Instruction
				EX_MEM.ALUOutput = ID_EX.A;
				break;
			case 0x18: //MULT, ALU Instruction
				if ((ID_EX.A & 0x80000000) == 0x80000000){
					p1 = 0xFFFFFFFF00000000 | ID_EX.A;
				}else{
					p1 = 0x00000000FFFFFFFF & ID_EX.A;
				}
				if ((ID_EX.B & 0x80000000) == 0x80000000){
					p2 = 0xFFFFFFFF00000000 | ID_EX.B;
				}else{
					p2 = 0x00000000FFFFFFFF & ID_EX.B;
				}
				product = p1 * p2;
				EX_MEM.LO = (product & 0X00000000FFFFFFFF);
				EX_MEM.HI = (product & 0XFFFFFFFF00000000)>>32;
				break;
			case 0x19: //MULTU, ALU Instruction
				product = (uint64_t)ID_EX.A * (uint64_t)ID_EX.B;
				EX_MEM.LO = (product & 0X00000000FFFFFFFF);
				EX_MEM.HI = (product & 0XFFFFFFFF00000000)>>32;
				break;
			case 0x1A: //DIV, ALU Instruction
				if(ID_EX.B != 0){
					EX_MEM.LO = (int32_t)ID_EX.A / (int32_t)ID_EX.B;
					EX_MEM.HI = (int32_t)ID_EX.A % (int32_t)ID_EX.B;
				}
				break;
			case 0x1B: //DIVU, ALU Instruction
				if(ID_EX.B != 0){
					EX_MEM.LO = ID_EX.A / ID_EX.B;
					EX_MEM.HI = ID_EX.A % ID_EX.B;
				}
				break;
			case 0x20: //ADD, ALU Instruction
				EX_MEM.ALUOutput = ID_EX.A + ID_EX.B;

				break;
			case 0x21: //ADDU, ALU Instruction
				EX_MEM.ALUOutput = ID_EX.A + ID_EX.B;

				break;
			case 0x22: //SUB, ALU Instruction
				EX_MEM.ALUOutput = ID_EX.A - ID_EX.B;

				break;
			case 0x23: //SUBU, ALU Instruction
				EX_MEM.ALUOutput = ID_EX.A - ID_EX.B;

				break;
			case 0x24: //AND, ALU Instruction
				EX_MEM.ALUOutput = ID_EX.A & ID_EX.B;

				break;
			case 0x25: //OR, ALU Instruction
				EX_MEM.ALUOutput = ID_EX.A | ID_EX.B;

				break;
			case 0x26: //XOR, ALU Instruction
				EX_MEM.ALUOutput = ID_EX.A ^ ID_EX.B;

				break;
			case 0x27: //NOR, ALU Instruction
				EX_MEM.ALUOutput = ~(ID_EX.A | ID_EX.B);
				break;
			case 0x2A: //SLT, ALU Instruction
				if(ID_EX.A < ID_EX.B){
					EX_MEM.ALUOutput = 0x1;
				}
				else{
					EX_MEM.ALUOutput = 0x0;
				}
				break;
			default:
				printf("Instruction at 0x%x is not implemented!\n", CURRENT_STATE.PC);
				break;
		}
	}
	else{
		switch(opcode){
			case 0x08: //ADDI, ALU Instruction
				EX_MEM.ALUOutput = ID_EX.A + ( (ID_EX.imm & 0x8000) > 0 ? (ID_EX.imm | 0xFFFF0000) : (ID_EX.imm & 0x0000FFFF));
				break;
			case 0x09: //ADDIU, ALU Instruction
				EX_MEM.ALUOutput = ID_EX.A + ( (ID_EX.imm & 0x8000) > 0 ? (ID_EX.imm | 0xFFFF0000) : (ID_EX.imm & 0x0000FFFF));
				break;
			case 0x0A: //SLTI, ALU Instruction
				if ( (  (int32_t)ID_EX.A - (int32_t)( (ID_EX.imm & 0x8000) > 0 ? (ID_EX.imm | 0xFFFF0000) : (ID_EX.imm & 0x0000FFFF))) < 0){
					EX_MEM.ALUOutput = 0x1;
				}
				else{
					EX_MEM.ALUOutput = 0x0;
				}
				break;
			case 0x0C: //ANDI, ALU Instruction
				EX_MEM.ALUOutput = ID_EX.A & (ID_EX.imm & 0x0000FFFF);
				break;
			case 0x0D: //ORI, ALU InstructionEM stage
ALU calculated insided stage 14 
ALU output insided stage 8 

				EX_MEM.ALUOutput = ID_EX.A | (ID_EX.imm & 0x0000FFFF);
				break;
			case 0x0E: //XORI, ALU Instruction
				EX_MEM.ALUOutput = ID_EX.A ^ (ID_EX.imm & 0x0000FFFF);
				break;
			case 0x0F: //LUI, Load/Store Instruction
				EX_MEM.ALUOutput = ID_EX.imm << 16;
				break;
			case 0x20: //LB, Load/Store Instruction
				EX_MEM.ALUOutput = ID_EX.A + ( (ID_EX.imm & 0x8000) > 0 ? (ID_EX.imm | 0xFFFF0000) : (ID_EX.imm & 0x0000FFFF)) ;
				break;
			case 0x21: //LH, Load/Store Instruction
				EX_MEM.ALUOutput = ID_EX.A + ( (ID_EX.imm & 0x8000) > 0 ? (ID_EX.imm | 0xFFFF0000) : (ID_EX.imm & 0x0000FFFF)) ;
				break;
			case 0x23: //LW, Load/Store Instruction
				EX_MEM.ALUOutput = ID_EX.A + ( (ID_EX.imm & 0x8000) > 0 ? (ID_EX.imm | 0xFFFF0000) : (ID_EX.imm & 0x0000FFFF)) ;
				break;
			case 0x28: //SB, Load/Store Instruction
				EX_MEM.ALUOutput = ID_EX.A + ( (ID_EX.imm & 0x8000) > 0 ? (ID_EX.imm | 0xFFFF0000) : (ID_EX.imm & 0x0000FFFF)) ;
				break;
			case 0x29: //SH, Load/Store Instruction
				EX_MEM.ALUOutput = ID_EX.A + ( (ID_EX.imm & 0x8000) > 0 ? (ID_EX.imm | 0xFFFF0000) : (ID_EX.imm & 0x0000FFFF)) ;
				break;
			case 0x2B: //SW, Load/Store Instruction
				printf("ALU calculated insided stage %x \n",EX_MEM.B);
				EX_MEM.ALUOutput = ID_EX.A + ( (ID_EX.imm & 0x8000) > 0 ? (ID_EX.imm | 0xFFFF0000) : (ID_EX.imm & 0x0000FFFF)) ;
 				printf("ALU output insided stage %x \n",EX_MEM.ALUOutput);
                                mem_write_32(EX_MEM.ALUOutput, EX_MEM.B);
			
				break;
			default:
				// put more things here
				printf("Instruction at 0x%x is not implemented!\n", CURRENT_STATE.PC);
				break;
		}
	}
}

/************************************************************/
/* instruction decode (ID) pipeline stage:                                                         */
/************************************************************/
void ID()
{
	/*IMPLEMENT THIS*/
	uint32_t rs;
	uint32_t rt;
	uint32_t immediate;

	rs = (IF_ID.IR & 0x03E00000) >> 21;
	rt = (IF_ID.IR & 0x001F0000) >> 16;
	immediate = IF_ID.IR & 0x0000FFFF;// use bit mask

	ID_EX.IR = IF_ID.IR;
	ID_EX.A = CURRENT_STATE.REGS[rs];
	ID_EX.B = CURRENT_STATE.REGS[rt];
	ID_EX.imm = immediate;
}

/************************************************************/
/* instruction fetch (IF) pipeline stage:                                                              */
/************************************************************/
void IF()
{
	IF_ID.IR = mem_read_32(CURRENT_STATE.PC);

	NEXT_STATE.PC = CURRENT_STATE.PC + 4;//correct
	IF_ID.PC = NEXT_STATE.PC;	
	//IF_ID.PC = CURRENT_STATE.PC + 4;
	/*IMPLEMENT THIS*/
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
	/*IMPLEMENT THIS*/
	int i;
	uint32_t addr;

	for(i=0; i<PROGRAM_SIZE; i++){
		addr = MEM_TEXT_BEGIN + (i*4);
		printf("[0x%x]\t", addr);
		print_instruction(addr);
	}
}

/************************************************************/
/* Print the current pipeline                                                                                    */
/************************************************************/
void show_pipeline(){
	/*IMPLEMENT THIS*/
	printf("\nCurrent PC:    ");
	print_instruction(CURRENT_STATE.PC-4);

	printf("\nIF_ID.IR:    ");
	printf("%x", IF_ID.IR);

	printf("\nIF_ID.PC:    ");
	printf("%x", IF_ID.PC);

	printf("\nID_EX.IR:    ");
	printf("%x", ID_EX.IR);

	printf("\nID_EX.A:    ");
	printf("%x", ID_EX.A);

	printf("\nID_EX.B:    ");
	printf("%x", ID_EX.B);

	printf("\nID_EX.imm:    ");
	printf("%x", ID_EX.imm);

	printf("\nEX_MEM.IR:    ");
	printf("%x", EX_MEM.IR);

	printf("\nEX_MEM.A:    ");
	printf("%x", EX_MEM.A);

	printf("\nEX_MEM.B:    ");
	printf("%x", EX_MEM.B);

	printf("\nEX_MEM.ALUOutput:    ");
	printf("%x", EX_MEM.ALUOutput);

	printf("\nMEM_WB.IR:    ");
	printf("%x", MEM_WB.IR);

	printf("\nMEM_WB.ALUOutput:    ");
	printf("%x", MEM_WB.ALUOutput);

	printf("\nMEM_WB.LMD:    ");
	printf("%x\n", MEM_WB.LMD);
}

void print_instruction(uint32_t addr){
	uint32_t instruction, opcode, function, rs, rt, rd, sa, immediate, target;

	instruction = mem_read_32(addr);

	opcode = (instruction & 0xFC000000) >> 26;
	function = instruction & 0x0000003F;
	rs = (instruction & 0x03E00000) >> 21;
	rt = (instruction & 0x001F0000) >> 16;
	rd = (instruction & 0x0000F800) >> 11;
	sa = (instruction & 0x000007C0) >> 6;
	immediate = instruction & 0x0000FFFF;
	target = instruction & 0x03FFFFFF;

	if(opcode == 0x00){
		/*R format instructions here*/

		switch(function){
			case 0x00:
				printf("SLL $r%u, $r%u, 0x%x\n", rd, rt, sa);
				break;
			case 0x02:
				printf("SRL $r%u, $r%u, 0x%x\n", rd, rt, sa);
				break;
			case 0x03:
				printf("SRA $r%u, $r%u, 0x%x\n", rd, rt, sa);
				break;
			case 0x08:
				printf("JR $r%u\n", rs);
				break;
			case 0x09:
				if(rd == 31){
					printf("JALR $r%u\n", rs);
				}
				else{
					printf("JALR $r%u, $r%u\n", rd, rs);
				}
				break;
			case 0x0C:
				printf("SYSCALL\n");
				break;
			case 0x10:
				printf("MFHI $r%u\n", rd);
				break;
			case 0x11:
				printf("MTHI $r%u\n", rs);
				break;
			case 0x12:
				printf("MFLO $r%u\n", rd);
				break;
			case 0x13:
				printf("MTLO $r%u\n", rs);
				break;
			case 0x18:
				printf("MULT $r%u, $r%u\n", rs, rt);
				break;
			case 0x19:
				printf("MULTU $r%u, $r%u\n", rs, rt);
				break;
			case 0x1A:
				printf("DIV $r%u, $r%u\n", rs, rt);
				break;
			case 0x1B:
				printf("DIVU $r%u, $r%u\n", rs, rt);
				break;
			case 0x20:
				printf("ADD $r%u, $r%u, $r%u\n", rd, rs, rt);
				break;
			case 0x21:
				printf("ADDU $r%u, $r%u, $r%u\n", rd, rs, rt);
				break;
			case 0x22:
				printf("SUB $r%u, $r%u, $r%u\n", rd, rs, rt);
				break;
			case 0x23:
				printf("SUBU $r%u, $r%u, $r%u\n", rd, rs, rt);
				break;
			case 0x24:
				printf("AND $r%u, $r%u, $r%u\n", rd, rs, rt);
				break;
			case 0x25:
				printf("OR $r%u, $r%u, $r%u\n", rd, rs, rt);
				break;
			case 0x26:
				printf("XOR $r%u, $r%u, $r%u\n", rd, rs, rt);
				break;
			case 0x27:
				printf("NOR $r%u, $r%u, $r%u\n", rd, rs, rt);
				break;
			case 0x2A:
				printf("SLT $r%u, $r%u, $r%u\n", rd, rs, rt);
				break;
			default:
				printf("Instruction is not implemented!\n");
				break;
		}
	}
	else{
		switch(opcode){
			case 0x01:
				if(rt == 0){
					printf("BLTZ $r%u, 0x%x\n", rs, immediate<<2);
				}
				else if(rt == 1){
					printf("BGEZ $r%u, 0x%x\n", rs, immediate<<2);
				}
				break;
			case 0x02:
				printf("J 0x%x\n", (addr & 0xF0000000) | (target<<2));
				break;
			case 0x03:
				printf("JAL 0x%x\n", (addr & 0xF0000000) | (target<<2));
				break;
			case 0x04:
				printf("BEQ $r%u, $r%u, 0x%x\n", rs, rt, immediate<<2);
				break;
			case 0x05:
				printf("BNE $r%u, $r%u, 0x%x\n", rs, rt, immediate<<2);
				break;
			case 0x06:
				printf("BLEZ $r%u, 0x%x\n", rs, immediate<<2);
				break;
			case 0x07:
				printf("BGTZ $r%u, 0x%x\n", rs, immediate<<2);
				break;
			case 0x08:
				printf("ADDI $r%u, $r%u, 0x%x\n", rt, rs, immediate);
				break;
			case 0x09:
				printf("ADDIU $r%u, $r%u, 0x%x\n", rt, rs, immediate);
				break;
			case 0x0A:
				printf("SLTI $r%u, $r%u, 0x%x\n", rt, rs, immediate);
				break;
			case 0x0C:
				printf("ANDI $r%u, $r%u, 0x%x\n", rt, rs, immediate);
				break;
			case 0x0D:
				printf("ORI $r%u, $r%u, 0x%x\n", rt, rs, immediate);
				break;
			case 0x0E:
				printf("XORI $r%u, $r%u, 0x%x\n", rt, rs, immediate);
				break;
			case 0x0F:
				printf("LUI $r%u, 0x%x\n", rt, immediate);
				break;
			case 0x20:
				printf("LB $r%u, 0x%x($r%u)\n", rt, immediate, rs);
				break;
			case 0x21:
				printf("LH $r%u, 0x%x($r%u)\n", rt, immediate, rs);
				break;
			case 0x23:
				printf("LW $r%u, 0x%x($r%u)\n", rt, immediate, rs);
				break;
			case 0x28:
				printf("SB $r%u, 0x%x($r%u)\n", rt, immediate, rs);
				break;
			case 0x29:
				printf("SH $r%u, 0x%x($r%u)\n", rt, immediate, rs);
				break;
			case 0x2B:
				printf("SW $r%u, 0x%x($r%u)\n", rt, immediate, rs);
				break;
			default:
				printf("Instruction is not implemented!\n");
				break;
		}
	}
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
