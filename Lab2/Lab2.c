//============================================================================
// Name        : ECE4270Lab2.cpp
// Author      : Yihe Wang
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

#define MAX_LINE 1024

void writeBubbleSort();
void writeFibonacci();
string transvertInstruction(string);
int *R_Type_Concatenate(int *, int *, int *, int *);
int *I_Type_Concatenate(int *, int *, int *, int *);
int *shift_Type_Concatenate(int *, int *, int *, int *);
int *load_Type_Concatenate(int *, int *, int *, int *);
int *move_Type_Concatenate(int *, int*);
int *branch_Less_Type_Concatenate(int *, int *, int *);
int *jump_Type_Concatenate(int *, int *);
string R_Type_Instruction(vector<string>, int*);
string I_Type_Instruction(vector<string>, int*);
string branch_Type_Instruction(vector<string>, int*);
vector<string> readNumber(vector<string>);
int* getPartialInstruction(string, int, bool);//take one line of code as input, and return one line of machine code
int *storeBinaryInArray(int, int);//take size and number as input, reture an int array to store binary number
string binaryToHex(int *);//take 32 bits binary array as input, get a hexidecimal output
char decimalToHex(int);//take a decimal as input, return a hexidecimal char
void printArray(int *, int);
//

void writeBubbleSort()
{
	vector<string> multipleLines;
	string fileName1 = "BubbleSort.txt";
	string fileName2 = "ResultOfBubbleSort.txt";
	ifstream in;
	ofstream out;
	in.open(fileName1);
	out.open(fileName2);
	if(!in.is_open()){
		cout<<"File BubbleSort.txt open error!"<<endl;
	}//check error
	if(!out.is_open()){
		cout<<"File ResultOfBubbleSort.txt open error!"<<endl;
	}//check error

	string oneLine;
	while(!in.eof()){
		//in>>oneLine;
		getline(in,oneLine);
		multipleLines.push_back(oneLine);
	}//write the signal into the vector
	cout<<multipleLines[0]<<endl;

	string instruction;
	int size = multipleLines.size();
	for(int i = 0; i < size; ++i){
		instruction = transvertInstruction(multipleLines[i]);
		cout<<multipleLines[i]<<endl;
		out<<instruction<<endl;
	}

	in.close();
	out.close();

}

void writeFibonacci()
{
	vector<string> multipleLines;
	string fileName1 = "Fibonacci.txt";
	string fileName2 = "ResultOfFibonacci.txt";
	ifstream in;
	ofstream out;
	in.open(fileName1);
	out.open(fileName2);
	if(!in.is_open()){
		cout<<"File Fibonacci.txt open error!"<<endl;
	}//check error
	if(!out.is_open()){
		cout<<"File ResultOfFibonacci.txt open error!"<<endl;
	}//check error

	string oneLine;
	while(!in.eof()){
		//in>>oneLine;
		getline(in,oneLine);
		multipleLines.push_back(oneLine);
	}//write the signal into the vector

	string instruction;
	int size = multipleLines.size();
	for(int i = 0; i < size; ++i){
		instruction = transvertInstruction(multipleLines[i]);
		out<<instruction<<endl;
	}

	in.close();
	out.close();
}

vector<string> readNumber(vector<string> result)
{
	int length = result.size();
	vector<string> array;
	string storage,temp, bracket;
	int counter= 1,i=0,t=0,j=0,flag=0;


	/*for (int j = 0; j < result.size(); j++) {
		cout<<result[j]<<endl;
	}
	cout<<"size:"<<result.size()<<endl;*/

	// for loop to read and judge each of number in the string
	for(i=1;i<length;i++)
	{
		for( t=0;t!=result[i].size();t++)
		{
			//flag is used to divide content in the bracket
			if(flag == 0)
			{
				// if loop check if charcter in the rang 0-9, a-e, A-E
				if( result[i][t]=='(' || (result[i][t]<='9') && (result[i][t]>='0'))
				{
					// if ther are number in the bracket then store it in string btacket
					if(result[i][t]=='(' )
					{
						while(result[i][t] != ')')
						{
							t++;                           //worke as counter to store every number in the bracket
							if(result[i][t] != ')')
							{
							bracket += result[i][t];
							}
							flag == 1;                     // set flag to prevent program retake content in the bracket
						}

					}
					if(flag == 0) // read and store number out of bracket
					{
						if(result[i][t] != '(' && result[i][t] != ')')   // dismiss '(' ')'
							{
							temp  += result[i][t];
							}
					}

				}
			}
			flag = 0;           //clear the flag for next string
		}
		//store data in the array
		array.push_back(temp) ;
		if(bracket.empty())
		{}
		else
		{
		array.push_back(bracket);
		}
		//clear string in case of retaking the number
		bracket.clear();
		temp.clear();
	}

	for (int j = 0; j < array.size(); j++) { //print the number
		cout<<array[j]<<" ";
	}
	cout<<endl;
	return array;
}

int* getPartialInstruction(string partialInstruction, int size, bool binaryOrNot)
{
	int number;
	if(binaryOrNot){
		number = stoi(partialInstruction,nullptr,2);
	}
	else{
		number = stoi(partialInstruction,nullptr,16);
	}
	//cout<<number<<endl;
	int *partialInstructionBinary = storeBinaryInArray(number,size);

	//get binary code for partial instruction, ex. rt is 5-bit
	return partialInstructionBinary;
}

int *storeBinaryInArray(int number, int size)
{
	int *binaryArray = (int*)malloc(sizeof(int)*size);
	int a, b;

	for(int i = size - 1; i >= 0; --i){
		a = number % 2;
		b = number / 2;
		number = b;
		binaryArray[i] =  a;
	}
	//return int array stored binary number
	return binaryArray;
}

//for instruction MFHI, MFLO
int *move_Type_Concatenate(int *rd, int *funct)
{
	int* thirtyTwoBits = (int *)malloc(sizeof(int)*32);
	for(int k = 0; k < 32; ++k){
		thirtyTwoBits[k] = 0;
	}

	int i;
	int j = 0;
	for(i = 16; i < 21; ++i){
		thirtyTwoBits[i] = rd[j];
		++j;
	}//store rd into binary array

	for(i = 26; i < 32; ++i){
		thirtyTwoBits[i] = funct[j];
		++j;
	}//store funct into binary array

	return thirtyTwoBits;
}

// for instruction LW, LB, LH, SW, SB, SH   , LUI is special case
int *load_Type_Concatenate(int *funct, int *base, int *rt, int *offset)
{
	int* thirtyTwoBits = (int *)malloc(sizeof(int)*32);
	for(int k = 0; k < 32; ++k){
		thirtyTwoBits[k] = 0;
	}

	int i;
	int j = 0;
	for(i = 0; i < 6; ++i){
		thirtyTwoBits[i] = funct[j];
		++j;
	}//store funct into binary array

	for(i = 6; i < 11; ++i){
		thirtyTwoBits[i] = base[j];
		++j;
	}//store base into binary array

	for(i = 11; i < 16; ++i){
		thirtyTwoBits[i] = rt[j];
		++j;
	}//store rt into binary array

	for(i = 16; i < 32; ++i){
		thirtyTwoBits[i] = offset[j];
		++j;
	}//store rt into binary array

	return thirtyTwoBits;
}

// for instruction SLL, SRL, SRA
int *shift_Type_Concatenate(int *rt, int *rd, int *sa, int *funct)
{
	int* thirtyTwoBits = (int *)malloc(sizeof(int)*32);
	for(int k = 0; k < 32; ++k){
		thirtyTwoBits[k] = 0;
	}

	int i;
	int j = 0;
	for(i = 11; i < 16; ++i){
		thirtyTwoBits[i] = rt[j];
		++j;
	}//store rt into binary array

	j = 0;
	for(i = 16; i < 21; ++i){
		thirtyTwoBits[i] = rd[j];
		++j;
	}//store rd into binary array

	j = 0;
	for(i = 21; i < 26; ++i){
		thirtyTwoBits[i] = sa[j];
		++j;
	}//store sa into binary array

	j = 0;
	for(i = 26; i < 32; ++i){
		thirtyTwoBits[i] = funct[j];
		++j;
	}//store funct into binary array

	return thirtyTwoBits;
}

//for instruction MULT, MULTU, DIV, DIVU
int *R_Type_Concatenate(int *rs, int *rt, int *funct)
{
	int* thirtyTwoBits = (int *)malloc(sizeof(int)*32);
	for(int k = 0; k < 32; ++k){
		thirtyTwoBits[k] = 0;
	}

	int i;
	int j = 0;
	for(i = 6; i < 11; ++i){
		thirtyTwoBits[i] = rs[j];
		++j;
	}//store rs into binary array

	j = 0;
	for(i = 11; i < 16; ++i){
		thirtyTwoBits[i] = rt[j];
		++j;
	}//store rt into binary array

	j = 0;
	for(i = 26; i < 32; ++i){
		thirtyTwoBits[i] = funct[j];
		++j;
	}//store funct into binary array

	return thirtyTwoBits;
}

int *R_Type_Concatenate(int *rs, int *rt, int *rd, int *funct)
{
	int* thirtyTwoBits = (int *)malloc(sizeof(int)*32);
	for(int k = 0; k < 32; ++k){
		thirtyTwoBits[k] = 0;
	}

	int i;
	int j = 0;
	for(i = 6; i < 11; ++i){
		thirtyTwoBits[i] = rs[j];
		++j;
	}//store rs into binary array

	j = 0;
	for(i = 11; i < 16; ++i){
		thirtyTwoBits[i] = rt[j];
		++j;
	}//store rt into binary array

	j = 0;
	for(i = 16; i < 21; ++i){
		thirtyTwoBits[i] = rd[j];
		++j;
	}//store rd into binary array

	j = 0;
	for(i = 26; i < 32; ++i){
		thirtyTwoBits[i] = funct[j];
		++j;
	}//store funct code into binary array

	return thirtyTwoBits;
}

int *I_Type_Concatenate(int *op, int *rs, int *rt, int *immediate)
{
	int* thirtyTwoBits = (int *)malloc(sizeof(int)*32);
	for(int k = 0; k < 32; ++k){
		thirtyTwoBits[k] = 0;
	}

	int i;
	int j = 0;
	for(i = 0; i < 6; ++i){
		thirtyTwoBits[i] = op[j];
		++j;
	}//store op into binary array

	j = 0;
	for(i = 6; i < 11; ++i){
		thirtyTwoBits[i] = rs[j];
		++j;
	}//store rs into binary array

	j = 0;
	for(i = 11; i < 16; ++i){
		thirtyTwoBits[i] = rt[j];
		++j;
	}//store rt into binary array

	j = 0;
	for(i = 16; i < 32; ++i){
		thirtyTwoBits[i] = immediate[j];
		++j;
	}//store rd into binary array

	return thirtyTwoBits;
}

// for instruction BLEZ, BLTZ(special case), BGEZ(special case)
int *branch_Less_Type_Concatenate(int *op, int *rs, int *offset)
{
	int* thirtyTwoBits = (int *)malloc(sizeof(int)*32);
	for(int k = 0; k < 32; ++k){
		thirtyTwoBits[k] = 0;
	}

	int i;
	int j = 0;
	for(i = 0; i < 6; ++i){
		thirtyTwoBits[i] = op[j];
		++j;
	}//store op into binary array

	j = 0;
	for(i = 6; i < 11; ++i){
		thirtyTwoBits[i] = rs[j];
		++j;
	}//store rs into binary array

	j = 0;
	for(i = 16; i < 32; ++i){
		thirtyTwoBits[i] = offset[j];
		++j;
	}//store rd into binary array

	return thirtyTwoBits;
}

//for instruction J, JAL
int *jump_Type_Concatenate(int *op, int *target)
{
	int* thirtyTwoBits = (int *)malloc(sizeof(int)*32);
	for(int k = 0; k < 32; ++k){
		thirtyTwoBits[k] = 0;
	}

	int i;
	int j = 0;
	for(i = 0; i < 6; ++i){
		thirtyTwoBits[i] = op[j];
		++j;
	}//store op into binary array

	j = 0;
	for(i = 6; i < 32; ++i){
		thirtyTwoBits[i] = target[j];
		++j;
	}//store target into binary array


	return thirtyTwoBits;
}

string R_Type_Instruction(vector<string> numberList, int* funct)
{
	int *rs;
	int *rd;
	int *rt;
	string machineInstruction;
	int *thirtyTwoBits;

	rd = getPartialInstruction(numberList[0], 5, false);
	rs = getPartialInstruction(numberList[1], 5, false);
	rt = getPartialInstruction(numberList[2], 5, false);
	thirtyTwoBits = R_Type_Concatenate(rs, rt, rd, funct);
	printArray(thirtyTwoBits,32);
	machineInstruction = binaryToHex(thirtyTwoBits);

	return machineInstruction;
}

string I_Type_Instruction(vector<string> numberList, int* op)
{
	int *rs;
	int *rt;
	int *immediate;
	string machineInstruction;
	int *thirtyTwoBits;

	rt = getPartialInstruction(numberList[0], 5, false);
	rs = getPartialInstruction(numberList[1], 5, false);
	immediate = getPartialInstruction(numberList[2], 16, false);
	thirtyTwoBits = I_Type_Concatenate(op, rs, rt, immediate);
	printArray(thirtyTwoBits,32);
	machineInstruction = binaryToHex(thirtyTwoBits);

	return machineInstruction;
}

//for instruction BEQ, BNE
string branch_Type_Instruction(vector<string> numberList, int* op)
{
	int *rs;
	int *rt;
	int *offset;
	string machineInstruction;
	int *thirtyTwoBits;

	rs = getPartialInstruction(numberList[0], 5, false);
	rt = getPartialInstruction(numberList[1], 5, false);
	offset = getPartialInstruction(numberList[2], 16, false);
	thirtyTwoBits = I_Type_Concatenate(op, rs, rt, offset);
	printArray(thirtyTwoBits,32);
	machineInstruction = binaryToHex(thirtyTwoBits);

	return machineInstruction;
}

string binaryToHex(int *thirtyTwoBits)
{
	string machineInstruction = "00000000";
	int i;
	for(i = 0; i < 8; ++i){
		int tempNum = 0;
		int j;
		for(j = 0; j < 4; ++j){
			if(j == 0){
				tempNum = thirtyTwoBits[j + 4*i] * 8 + tempNum;
			}
			else if(j == 1){
				tempNum = thirtyTwoBits[j + 4*i] * 4 + tempNum;
			}
			else if(j == 2){
				tempNum = thirtyTwoBits[j + 4*i] * 2 + tempNum;
			}
			else if(j == 3){
				tempNum = thirtyTwoBits[j + 4*i] * 1 + tempNum;
			}
		}//transvert 4 bits into a decimal number
		//cout<<tempNum<<endl;
		machineInstruction[i] = decimalToHex(tempNum);
	}//every 4 bits as a loop

	return machineInstruction;
}

char decimalToHex(int tempNum)
{
	char *hex = (char *)malloc(sizeof(char));
	if(tempNum >= 10){
		switch(tempNum){
			case 10:
				hex[0] = 'A';
				break;
			case 11:
				hex[0] = 'B';
				break;
			case 12:
				hex[0] = 'C';
				break;
			case 13:
				hex[0] = 'D';
				break;
			case 14:
				hex[0] = 'E';
				break;
			case 15:
				hex[0] = 'F';
				break;
		}
	}// number bigger than 9
	else{
		sprintf(hex,"%d",tempNum);
	}//number smaller than 9

	return hex[0];
}

void printArray(int *binaryArray, int size)
{
	int i;
	for(i = 0; i < size - 1; ++i){
		cout<<binaryArray[i];
	}
	cout<<binaryArray[i]<<endl;
}

string transvertInstruction(string oneLine)
{
	//string instruction;// Like ADD, ADDI
	int *funct = (int*)malloc(sizeof(int)*6);
	int *op = (int*)malloc(sizeof(int)*6);
	int *rs;
	int *rd;
	int *rt;
	int *sa;
	int *base;
	int *offset;
	int *immediate;
	int *target;
	int *thirtyTwoBits;
	string machineInstruction;
	string tempResult;
	vector<string> result;//store the seperated strings
	stringstream input(oneLine);
	vector<string> numberList;

	while(input>>tempResult){
		result.push_back(tempResult);
	}// get the seperated strings and put into vector result

	//cout<<result[0]<<endl;

	if(result[0] == "ADD"){
		cout<<"Instruction ADD start"<<endl;
		numberList = readNumber(result);
		funct = getPartialInstruction("100000", 6, true);// ADD is 10 0000
		machineInstruction = R_Type_Instruction(numberList,funct);
	}
	else if(result[0] == "ADDU"){
		cout<<"Instruction ADDU start"<<endl;
		numberList = readNumber(result);
		funct = getPartialInstruction("100001", 6, true);// ADDU is 10 0001
		machineInstruction = R_Type_Instruction(numberList,funct);
	}
	else if(result[0] == "ADDI"){
		cout<<"Instruction ADDI start"<<endl;
		numberList = readNumber(result);
		op = getPartialInstruction("001000", 6, true);// ADD is 10 0000
		machineInstruction = I_Type_Instruction(numberList,op);
	}
	else if(result[0] == "ADDIU"){
		cout<<"Instruction ADDIU start"<<endl;
		numberList = readNumber(result);
		op = getPartialInstruction("001001", 6, true);// ADDIU is 00 1001
		machineInstruction = I_Type_Instruction(numberList,op);
	}
	else if(result[0] == "SUB"){
		cout<<"Instruction SUB start"<<endl;
		numberList = readNumber(result);
		funct = getPartialInstruction("100010", 6, true);// SUB is 10 0010
		machineInstruction = R_Type_Instruction(numberList,funct);
	}
	else if(result[0] == "SUBU"){
		cout<<"Instruction SUBU"<<endl;
		numberList = readNumber(result);
		funct = getPartialInstruction("100011", 6, true);// SUB is 10 0011
		machineInstruction = R_Type_Instruction(numberList,funct);
	}
	else if(result[0] == "MULT"){
		cout<<"Instruction MULT start"<<endl;
		numberList = readNumber(result);
		rs = getPartialInstruction(numberList[0], 5, false);
		rt = getPartialInstruction(numberList[1], 5, false);
		funct = getPartialInstruction("011000", 6, true);// MULT is 01 1000
		thirtyTwoBits = R_Type_Concatenate(rs, rt, funct);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}
	else if(result[0] == "MULTU"){
		cout<<"Instruction MULTU start"<<endl;
		numberList = readNumber(result);
		rs = getPartialInstruction(numberList[0], 5, false);
		rt = getPartialInstruction(numberList[1], 5, false);
		funct = getPartialInstruction("011001", 6, true);// MULTU is 01 1001
		thirtyTwoBits = R_Type_Concatenate(rs, rt, funct);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}
	else if(result[0] == "DIV"){
		cout<<"Instruction DIV start"<<endl;
		numberList = readNumber(result);
		rs = getPartialInstruction(numberList[0], 5, false);
		rt = getPartialInstruction(numberList[1], 5, false);
		funct = getPartialInstruction("011010", 6, true);// DIV is 01 1010
		thirtyTwoBits = R_Type_Concatenate(rs, rt, funct);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}
	else if(result[0] == "DIVU"){
		cout<<"Instruction DIVU start"<<endl;
		numberList = readNumber(result);
		rs = getPartialInstruction(numberList[0], 5, false);
		rt = getPartialInstruction(numberList[1], 5, false);
		funct = getPartialInstruction("011011", 6, true);// DIVU is 01 1011
		thirtyTwoBits = R_Type_Concatenate(rs, rt, funct);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}
	else if(result[0] == "AND"){
		cout<<"Instruction AND start"<<endl;
		numberList = readNumber(result);
		funct = getPartialInstruction("100100", 6, true);// AND is 10 0100
		machineInstruction = R_Type_Instruction(numberList,funct);
	}
	else if(result[0] == "ANDI"){
		cout<<"Instruction ANDI start"<<endl;
		numberList = readNumber(result);
		op = getPartialInstruction("001100", 6, true);// ANDI is 00 1100
		machineInstruction = I_Type_Instruction(numberList,op);
	}
	else if(result[0] == "OR"){
		cout<<"Instruction OR start"<<endl;
		numberList = readNumber(result);
		funct = getPartialInstruction("100101", 6, true);// OR is 10 0101
		machineInstruction = R_Type_Instruction(numberList,funct);
	}
	else if(result[0] == "ORI"){
		cout<<"Instruction ORI start"<<endl;
		numberList = readNumber(result);
		op = getPartialInstruction("001101", 6, true);// ORI is 00 1101
		machineInstruction = I_Type_Instruction(numberList,op);
	}
	else if(result[0] == "XOR"){
		cout<<"Instruction XOR start"<<endl;
		numberList = readNumber(result);
		funct = getPartialInstruction("100110", 6, true);// XOR is 10 0110
		machineInstruction = R_Type_Instruction(numberList,funct);
	}
	else if(result[0] == "XORI"){
		cout<<"Instruction XORI"<<endl;
		numberList = readNumber(result);
		op = getPartialInstruction("001110", 6, true);// ORI is 00 1110
		machineInstruction = I_Type_Instruction(numberList,op);
	}
	else if(result[0] == "NOR"){
		cout<<"Instruction NOR start"<<endl;
		numberList = readNumber(result);
		funct = getPartialInstruction("100111", 6, true);// OR is 10 0111
		machineInstruction = R_Type_Instruction(numberList,funct);
	}
	else if(result[0] == "SLT"){
		cout<<"Instruction SLT start"<<endl;
		numberList = readNumber(result);
		funct = getPartialInstruction("101010", 6, true);// OR is 10 1010
		machineInstruction = R_Type_Instruction(numberList,funct);
	}
	else if(result[0] == "SLTI"){
		cout<<"Instruction SLTI"<<endl;
		numberList = readNumber(result);
		op = getPartialInstruction("001010", 6, true);// ORI is 00 1010
		machineInstruction = I_Type_Instruction(numberList,op);
	}
	else if(result[0] == "SLL"){
		cout<<"Instruction SLL"<<endl;
		numberList = readNumber(result);
		rd = getPartialInstruction(numberList[0], 5, false);
		rt = getPartialInstruction(numberList[1], 5, false);
		sa = getPartialInstruction(numberList[2], 5, false);
		funct = getPartialInstruction("000000", 6, true);// SLL is 00 0000
		thirtyTwoBits = shift_Type_Concatenate(rt, rd, sa, funct);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}
	else if(result[0] == "SRL"){
		cout<<"Instruction SRL"<<endl;
		numberList = readNumber(result);
		rd = getPartialInstruction(numberList[0], 5, false);
		rt = getPartialInstruction(numberList[1], 5, false);
		sa = getPartialInstruction(numberList[2], 5, false);
		funct = getPartialInstruction("000010", 6, true);// SRL is 00 0010
		thirtyTwoBits = shift_Type_Concatenate(rt, rd, sa, funct);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}
	else if(result[0] == "SRA"){
		cout<<"Instruction SRA"<<endl;
		numberList = readNumber(result);
		rd = getPartialInstruction(numberList[0], 5, false);
		rt = getPartialInstruction(numberList[1], 5, false);
		sa = getPartialInstruction(numberList[2], 5, false);
		funct = getPartialInstruction("000011", 6, true);// SRA is 00 0011
		thirtyTwoBits = shift_Type_Concatenate(rt, rd, sa, funct);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}
	else if(result[0] == "LW"){
		cout<<"Instruction LW"<<endl;
		numberList = readNumber(result);
		rt = getPartialInstruction(numberList[0], 5, false);
		offset = getPartialInstruction(numberList[1], 16, false);
		base = getPartialInstruction(numberList[2], 5, false);
		funct = getPartialInstruction("100011", 6, true);// LW is 10 0011
		thirtyTwoBits = load_Type_Concatenate(funct, base, rt, offset);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}
	else if(result[0] == "LB"){
		cout<<"Instruction LB"<<endl;
		numberList = readNumber(result);
		rt = getPartialInstruction(numberList[0], 5, false);
		offset = getPartialInstruction(numberList[1], 16, false);
		base = getPartialInstruction(numberList[2], 5, false);
		funct = getPartialInstruction("100000", 6, true);// LB is 10 0000
		thirtyTwoBits = load_Type_Concatenate(funct, base, rt, offset);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}
	else if(result[0] == "LH"){
		cout<<"Instruction LH"<<endl;
		numberList = readNumber(result);
		rt = getPartialInstruction(numberList[0], 5, false);
		offset = getPartialInstruction(numberList[1], 16, false);
		base = getPartialInstruction(numberList[2], 5, false);
		funct = getPartialInstruction("100001", 6, true);// LB is 10 0001
		thirtyTwoBits = load_Type_Concatenate(funct, base, rt, offset);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}

	//special case, I use it as load type
	else if(result[0] == "LUI"){
		cout<<"Instruction LUI"<<endl;
		numberList = readNumber(result);
		rt = getPartialInstruction(numberList[0], 5, false);
		immediate = getPartialInstruction(numberList[1], 16, false);
		base = getPartialInstruction("00000", 5, true);
		funct = getPartialInstruction("001111", 6, true);// LUI is 00 1111
		thirtyTwoBits = load_Type_Concatenate(funct, base, rt, immediate);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}
	else if(result[0] == "SW"){
		cout<<"Instruction SW"<<endl;
		numberList = readNumber(result);
		rt = getPartialInstruction(numberList[0], 5, false);
		offset = getPartialInstruction(numberList[1], 16, false);
		base = getPartialInstruction(numberList[2], 5, false);
		funct = getPartialInstruction("101011", 6, true);// SW is 10 1011
		thirtyTwoBits = load_Type_Concatenate(funct, base, rt, offset);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}
	else if(result[0] == "SB"){
		cout<<"Instruction SB"<<endl;
		numberList = readNumber(result);
		rt = getPartialInstruction(numberList[0], 5, false);
		offset = getPartialInstruction(numberList[1], 16, false);
		base = getPartialInstruction(numberList[2], 5, false);
		funct = getPartialInstruction("101000", 6, true);// SB is 10 1000
		thirtyTwoBits = load_Type_Concatenate(funct, base, rt, offset);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}
	else if(result[0] == "SH"){
		cout<<"Instruction SH"<<endl;
		numberList = readNumber(result);
		rt = getPartialInstruction(numberList[0], 5, false);
		offset = getPartialInstruction(numberList[1], 16, false);
		base = getPartialInstruction(numberList[2], 5, false);
		funct = getPartialInstruction("101001", 6, true);// SH is 10 1001
		thirtyTwoBits = load_Type_Concatenate(funct, base, rt, offset);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}
	else if(result[0] == "MFHI"){
		cout<<"Instruction MFHI"<<endl;
		rd = getPartialInstruction(numberList[0], 5, false);
		funct = getPartialInstruction("010000", 6, true);// MFHI is 01 0000
		thirtyTwoBits = move_Type_Concatenate(rd, funct);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}
	else if(result[0] == "MFLO"){
		cout<<"Instruction MFLO"<<endl;
		rd = getPartialInstruction(numberList[0], 5, false);
		funct = getPartialInstruction("010010", 6, true);// MFLO is 01 0010
		thirtyTwoBits = move_Type_Concatenate(rd, funct);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}
	else if(result[0] == "BEQ"){
		cout<<"Instruction BEQ"<<endl;
		numberList = readNumber(result);
		op = getPartialInstruction("000100", 6, true);// BEQ is 00 0100
		machineInstruction = branch_Type_Instruction(numberList,op);
	}
	else if(result[0] == "BNE"){
		cout<<"Instruction BNE"<<endl;
		numberList = readNumber(result);
		op = getPartialInstruction("000101", 6, true);// BNE is 00 0101
		machineInstruction = branch_Type_Instruction(numberList,op);
	}
	else if(result[0] == "BLEZ"){
		cout<<"Instruction BLEZ"<<endl;
		rs = getPartialInstruction(numberList[0], 5, false);
		offset = getPartialInstruction(numberList[1], 16, false);
		op = getPartialInstruction("000110", 6, true);// BLEZ is 00 0110
		thirtyTwoBits = branch_Less_Type_Concatenate(op, rs, offset);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}
	else if(result[0] == "BLTZ"){
		cout<<"Instruction BLTZ"<<endl;
		//Special case: REGIMM:00 0001
		rs = getPartialInstruction(numberList[0], 5, false);
		offset = getPartialInstruction(numberList[1], 16, false);
		op = getPartialInstruction("000001", 6, true);// BLTZ is 00 0001
		thirtyTwoBits = branch_Less_Type_Concatenate(op, rs, offset);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}
	else if(result[0] == "BGEZ"){
		cout<<"Instruction BGEZ"<<endl;
		//Special case: REGIMM:00 0001 BGEZ:00001
		rs = getPartialInstruction(numberList[0], 5, false);
		offset = getPartialInstruction(numberList[1], 16, false);
		op = getPartialInstruction("000001", 6, true);// BGEZ is 00 0001
		thirtyTwoBits = branch_Less_Type_Concatenate(op, rs, offset);
		thirtyTwoBits[15] = 1;
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}
	else if(result[0] == "BGTZ"){
		cout<<"Instruction BGTZ"<<endl;
		rs = getPartialInstruction(numberList[0], 5, false);
		offset = getPartialInstruction(numberList[1], 16, false);
		op = getPartialInstruction("000111", 6, true);// BGTZ is 00 0111
		thirtyTwoBits = branch_Less_Type_Concatenate(op, rs, offset);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}
	else if(result[0] == "J"){
		cout<<"Instruction J"<<endl;
		target = getPartialInstruction(numberList[0], 26, false);
		op = getPartialInstruction("000010", 6, true);// J is 00 0010
		thirtyTwoBits = jump_Type_Concatenate(op, target);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}
	else if(result[0] == "JAL"){
		cout<<"Instruction JAL"<<endl;
		target = getPartialInstruction(numberList[0], 26, false);
		op = getPartialInstruction("000011", 6, true);// JAL is 00 0011
		thirtyTwoBits = jump_Type_Concatenate(op, target);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}

	//two special case, use R_Type_Instruction
	else if(result[0] == "JR"){
		cout<<"Instruction JR"<<endl;
		rs = getPartialInstruction(numberList[0], 5, false);
		rt = getPartialInstruction("00000", 5, true);
		rd = getPartialInstruction("00000", 5, true);
		funct = getPartialInstruction("001000", 6, true);// JR is 00 1000
		thirtyTwoBits = R_Type_Concatenate(rs, rt, rd, funct);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}
	//this one may not correct
	else if(result[0] == "JALR"){
		cout<<"Instruction JALR"<<endl;
		rd = getPartialInstruction(numberList[0], 5, false);
		rs = getPartialInstruction(numberList[1], 5, false);
		rt = getPartialInstruction("00000", 5, true);
		funct = getPartialInstruction("001001", 6, true);// JALR is 00 1001
		thirtyTwoBits = R_Type_Concatenate(rs, rt, rd, funct);
		printArray(thirtyTwoBits,32);
		machineInstruction = binaryToHex(thirtyTwoBits);
	}

	return machineInstruction;
}


int main() {
	writeBubbleSort();
	//writeFibonacci();

	//string oneLine = "ADD $7, $6, $6";
	/*string oneLine = "ADDI $5, $0, 255";
	string machineInstruction;
	machineInstruction = transvertInstruction(oneLine);
	cout<<machineInstruction<<endl;*/
	return 0;
}




