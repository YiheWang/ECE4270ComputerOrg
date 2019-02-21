#include <iostream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

void writeBubbleSort();
void writeFibonacci();
string transvertInstruction(string);
int concatenateThree(int, int, int);
//take one line of code as input, and return one line of machine code

int main() {
	//writeBubbleSort();
	//writeFibonacci();

	string oneLine = "ADD $7, $6, $6";
	string machineInstruction;
	machineInstruction = transvertInstruction(oneLine);
	return 0;
}

void writeBubbleSort()
{

}

void writeFibonacci()
{

}

int concatenateThree(int num1, int num2, int num3)
{
	int num = 1;

	return num;
}

string transvertInstruction(string oneLine)
{
	//string instruction;// Like ADD, ADDI
	string machineInstruction;
	string tempResult;
	vector<string> result;//store the seperated strings
	stringstream input(oneLine);

	while(input>>tempResult){
		result.push_back(tempResult);
	}// get the seperated strings and put into vector result

	cout<<result[0]<<endl;

	if(result[0] == "ADD"){
		cout<<"Instruction ADD"<<endl;
	}
	else if(result[0] == "ADDU"){
		cout<<"Instruction ADDU"<<endl;
	}
	else if(result[0] == "ADDI"){
		cout<<"Instruction ADDI"<<endl;
	}
	else if(result[0] == "ADDIU"){
		cout<<"Instruction ADDIU"<<endl;
	}
	else if(result[0] == "SUB"){
		cout<<"Instruction SUB"<<endl;
	}
	else if(result[0] == "SUBU"){
		cout<<"Instruction SUBU"<<endl;
	}
	else if(result[0] == "MULT"){
		cout<<"Instruction MULT"<<endl;
	}
	else if(result[0] == "MULTU"){
		cout<<"Instruction MULTU"<<endl;
	}
	else if(result[0] == "DIV"){
		cout<<"Instruction DIV"<<endl;
	}
	else if(result[0] == "DIVU"){
		cout<<"Instruction DIVU"<<endl;
	}
	else if(result[0] == "AND"){
		cout<<"Instruction AND"<<endl;
	}
	else if(result[0] == "ANDI"){
		cout<<"Instruction ANDI"<<endl;
	}
	else if(result[0] == "DIVU"){
		cout<<"Instruction DIVU"<<endl;
	}
	else if(result[0] == "OR"){
		cout<<"Instruction OR"<<endl;
	}
	else if(result[0] == "ORI"){
		cout<<"Instruction ORI"<<endl;
	}
	else if(result[0] == "XOR"){
		cout<<"Instruction XOR"<<endl;
	}
	else if(result[0] == "XORI"){
		cout<<"Instruction XORI"<<endl;
	}
	else if(result[0] == "NOR"){
		cout<<"Instruction NOR"<<endl;
	}
	else if(result[0] == "SLT"){
		cout<<"Instruction SLT"<<endl;
	}
	else if(result[0] == "SLTI"){
		cout<<"Instruction SLTI"<<endl;
	}
	else if(result[0] == "SLL"){
		cout<<"Instruction SLL"<<endl;
	}
	else if(result[0] == "SRL"){
		cout<<"Instruction SRL"<<endl;
	}
	else if(result[0] == "SRA"){
		cout<<"Instruction SRA"<<endl;
	}
	else if(result[0] == "LW"){
		cout<<"Instruction LW"<<endl;
	}
	else if(result[0] == "LB"){
		cout<<"Instruction LB"<<endl;
	}
	else if(result[0] == "LH"){
		cout<<"Instruction LH"<<endl;
	}
	else if(result[0] == "LUI"){
		cout<<"Instruction LUI"<<endl;
	}
	else if(result[0] == "SW"){
		cout<<"Instruction SW"<<endl;
	}
	else if(result[0] == "SB"){
		cout<<"Instruction SB"<<endl;
	}
	else if(result[0] == "SH"){
		cout<<"Instruction SH"<<endl;
	}
	else if(result[0] == "MFHI"){
		cout<<"Instruction MFHI"<<endl;
	}
	else if(result[0] == "MFLO"){
		cout<<"Instruction MFLO"<<endl;
	}
	else if(result[0] == "BEQ"){
		cout<<"Instruction BEQ"<<endl;
	}
	else if(result[0] == "BNE"){
		cout<<"Instruction BNE"<<endl;
	}
	else if(result[0] == "BLEZ"){
		cout<<"Instruction BLEZ"<<endl;
	}
	else if(result[0] == "BLTZ"){
		cout<<"Instruction BLTZ"<<endl;
	}
	else if(result[0] == "BGEZ"){
		cout<<"Instruction BGEZ"<<endl;
	}
	else if(result[0] == "BLTZ"){
		cout<<"Instruction BLTZ"<<endl;
	}
	else if(result[0] == "BGTZ"){
		cout<<"Instruction BGTZ"<<endl;
	}
	else if(result[0] == "J"){
		cout<<"Instruction J"<<endl;
	}
	else if(result[0] == "JR"){
		cout<<"Instruction JR"<<endl;
	}
	else if(result[0] == "JAL"){
		cout<<"Instruction JAL"<<endl;
	}
	else if(result[0] == "JALR"){
		cout<<"Instruction JALR"<<endl;
	}

	return machineInstruction;
}






