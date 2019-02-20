#include <iostream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

void writeBubbleSort();
void writeFibonacci();
string transvertInstruction(string);
//take one line of code as input, and return one line of machine code

int main() {
	writeBubbleSort();
	writeFibonacci();
	/*string s1 = "Hello World";
	if(isspace(s1[5])){
		cout<<"True"<<endl;
	}
	else{
		cout<<"False"<<endl;
	}*/
	return 0;
}

void writeBubbleSort()
{

}

void writeFibonacci()
{

}

string tranvertInstruction(string oneLine)
{
	//string instruction;// Like ADD, ADDI
	string tempResult;
	vector<string> result;//store the seperated strings
	stringstream input(oneLine);

	while(input>>tempResult){
		result.push_back(tempResult);
	}// get the seperated strings and put into vector result

	switch(vector[0]){
		case "ADD":
			cout<<"Instruction ADD"<<endl;
			break;
		case "ADDU":
			cout<<"Instruction ADDU"<<endl;
			break;
		case "ADDI":
			cout<<"Instruction ADDI"<<endl;
			break;
		case "ADDIU":
			cout<<"Instruction ADDIU"<<endl;
			break;
		case "SUB":
			cout<<"Instruction SUB"<<endl;
			break;
		case "SUBU":
			cout<<"Instruction SUBU"<<endl;
			break;
		case "ADDU":
			cout<<"Instruction ADDU"<<endl;
			break;
		case "ADDU":
			cout<<"Instruction ADDU"<<endl;
			break;
		case "ADDU":
			cout<<"Instruction ADDU"<<endl;
			break;
		case "ADDU":
			cout<<"Instruction ADDU"<<endl;
			break;
		case "ADDU":
			cout<<"Instruction ADDU"<<endl;
			break;
		case "ADDU":
			cout<<"Instruction ADDU"<<endl;
			break;
		case "ADDU":
			cout<<"Instruction ADDU"<<endl;
			break;
		case "ADDU":
			cout<<"Instruction ADDU"<<endl;
			break;
		case "ADDU":
			cout<<"Instruction ADDU"<<endl;
			break;
		case "ADDU":
			cout<<"Instruction ADDU"<<endl;
			break;
		case "ADDU":
			cout<<"Instruction ADDU"<<endl;
			break;
		case "ADDU":
			cout<<"Instruction ADDU"<<endl;
			break;
		case "ADDU":
			cout<<"Instruction ADDU"<<endl;
			break;
		case "ADDU":
			cout<<"Instruction ADDU"<<endl;
			break;
		case "ADDU":
			cout<<"Instruction ADDU"<<endl;
			break;
	}
}



