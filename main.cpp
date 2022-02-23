#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
using namespace std;

// structure of token
struct token {
	int token_no;
	string lexeme;
	int lineNo;
};

static token createToken(int token_num, string lex, int line){
	token tok;
	tok.token_no = token_num;
	tok.lexeme = lex;
	tok.lineNo = line;
	return tok;
}

vector<token> token_list;
vector<string> Keywords = {
	"main", "int", "float", "string", "bool", "true", "false", "print", "for", "while", "var", "if", "elif", "else", "return"
};

// stray character error
void throw_error(char ch, int lineNo){
	cout<<"Lexical error: char "<< "'" << ch << "'" << " in program at line number = "<< lineNo <<endl;
}

// stray lexeme error
void throw_error(string lex, int *index, int lineNo){
	cout<<"Lexical error: char "<< "'" << lex << "'" << " in program at line number = "<< lineNo <<endl;
    (*index)++;
}

// ignore comments while reading input file
void Skip_Comments(string line, int *index, int lineNo){
	(*index)++;
	if(line[*index] == '%') {
		while(line[*index]) (*index)++;
	}
	else{
		(*index)--;
		throw_error("%", index, lineNo);
		return;
	}
}

// for arithmetic operators
void Arithmetic_Op(string line, int *index, int lineNo){
	switch(line[*index]){
		case '+': token_list.push_back(createToken(300, "+", lineNo)); break;
		case '-': token_list.push_back(createToken(301, "-", lineNo)); break;
		case '*': token_list.push_back(createToken(302, "*", lineNo)); break;
		case '/': token_list.push_back(createToken(303, "/", lineNo)); break;
	}
	(*index)++;
}

// for relational operators
void Relational_Op(string line,int *index,int lineNo){
	switch(line[*index]){
		case '<':
			(*index)++;
			if(line[*index] == '='){
				token_list.push_back(createToken(500, "<=", lineNo));
			}
			else{
				(*index)--;
				token_list.push_back(createToken(501, "<", lineNo));
			}
			break;

		case '>':
			(*index)++;
			if(line[*index]=='='){
				token_list.push_back(createToken(502, ">=",lineNo));
			}
			else{
				(*index)--;
				token_list.push_back(createToken(503, ">",lineNo));
			}
			break;

		case '=':
			(*index)++;
		 	if(line[*index]=='='){
		   		token_list.push_back(createToken(504, "==", lineNo));
			}
         	else if(line[*index] == ' '){
         		throw_error("=",index,lineNo);
         		return;
        	}
         	else{
         		(*index)--;
         		throw_error("=", index, lineNo);
         		return;
         	}
			break;

	}
	(*index)++;
}

// for assignment operators
void Assignment_Op(string line, int* index, int lineNo){
	(*index)++;

	if(line[*index] == '=') {
		token_list.push_back(createToken(800, ":=", lineNo));
	} else if(line[*index] == ' '){
		throw_error(":", index, lineNo);
	    return;
	} else {
		(*index)--;
		throw_error(":", index, lineNo);
		return;
	}

	(*index)++;
}

// for boolean operators
void Boolean_Op(string line, int* index, int lineNo) {
	string lexeme = "";
	lexeme += line[*index];
	if(line[*index] == line[(*index) + 1]) { // for && and ||
		lexeme += line[*index];
		(*index)++;
		(*index)++;
		token_list.push_back(createToken(400, lexeme, lineNo));
	} else {
		throw_error(lexeme, index, lineNo);
		return;
	}
}

// scanning delimiters
void Delimiter(string line, int* index, int lineNo){
	switch(line[*index]){
		case '{': token_list.push_back(createToken(600, "{", lineNo)); break;
		case '}': token_list.push_back(createToken(601, "}", lineNo)); break;
		case '(': token_list.push_back(createToken(602, "(", lineNo)); break;
		case ')': token_list.push_back(createToken(603, ")", lineNo)); break;
		case ',': token_list.push_back(createToken(604, ",", lineNo)); break;
		case '[': token_list.push_back(createToken(605, "[", lineNo)); break;
		case ']': token_list.push_back(createToken(606, "]", lineNo)); break;
	}

	(*index)++;
}

// DFA for scanning numbers
void Number_token(string line, int* index, int lineNo){
	int state = 0; //start state
	string lexeme = "";

	if(line[*index] < '0' || line[*index] > '9'){
		// Not a number token, throw error
		(*index)--;
		state = 3;
		throw_error(lexeme, index, lineNo);
		return;
	}

	// add all the digits
	while(line[*index] >= '0' && line[*index] <= '9'){
		lexeme += line[*index];
		(*index)++;
	}

	// check decimals
	if(line[*index] == '.'){
		state = 1;
		lexeme += line[*index];
		(*index)++;
	}
	else {
		// 100 - integers
		token_list.push_back(createToken(100, lexeme, lineNo));
	}

    if(state == 1){
		// decimal found
    	if(line[*index] < '0' || line[*index] > '9'){
    		state = 3;
    		throw_error(lexeme, index, lineNo);
    		(*index)--;
    		return;
    	}
    	else{
    		state = 2;
    		lexeme += line[*index];
    		(*index)++;
    	}
    }

    if(state == 2){
		// add the remaining numbers, no decimal character expected next
    	while(line[*index] >= '0' && line[*index] <= '9'){
            lexeme += line[*index];
    		(*index)++;
    	}
		// 101 - floats
    	token_list.push_back(createToken(101, lexeme, lineNo));
    }
}

// dfa for scanning named entities
void Scan_Identifiers(string line, int* index, int lineNo){
	string lexeme = "";
	while((line[*index]) && ((line[*index] >= 'a' && line[*index] <= 'z') || (line[*index] >= '0' && line[*index] <= '9') || (line[*index] >= 'A' && line[*index] <= 'Z'))){
        lexeme += line[*index];
		(*index)++;
	}

	int flg = 0;
	for(int i = 0; i < Keywords.size(); i++){
		if(lexeme == Keywords[i]){
			// 201 + i -> keywords
			token_list.push_back(createToken(201 + i, lexeme, lineNo));
			flg = 1;
			break;
		}
	}
	// 200 -> identifier
	if(flg == 0) token_list.push_back(createToken(200, lexeme, lineNo));
}

int main(){
	ifstream file_in; // input file stream
	ofstream file_out; // output file stream

	string inputFilePath = "C:/Users/iamth/Documents/GitHub/Compiler-Construction-Assignments-main/Compiler-Construction-Assignments-main/TestCases/tc_4.txt";
	string outputFilePath = "C:/Users/iamth/Documents/GitHub/Compiler-Construction-Assignments-main/Compiler-Construction-Assignments-main/TestCases/tc_4_out.txt";

	// open files
	file_in.open(inputFilePath);
	file_out.open(outputFilePath);

    int lineNo = 0; // line number count
	string line;

	while(getline(file_in, line)){
		lineNo++;
		int index = 0;

		while(line[index]){
			if((line[index] >= '0' && line[index] <= '9')) {
				// scan number literals
				Number_token(line, &index, lineNo); // 100 series
			} else if((line[index] >= 'A' && line[index] <= 'Z') || (line[index] >= 'a' && line[index] <= 'z')){
				// scan characters literals (identifiers)
				Scan_Identifiers(line, &index, lineNo); // 200
			} else {
				switch(line[index]){
					case '%': Skip_Comments(line, &index, lineNo); break; // ignoring comments
					case '\t':
					case ' ': index++; break; // ignore tabs and whitespaces
					case '*':
					case '/': 
					case '+':
					case '-': Arithmetic_Op(line, &index, lineNo); break; // 300 // scan arithmetic operators
					case '|':
					case '&': Boolean_Op(line, &index, lineNo); break; // 400 // Boolean operators - && and ||
					case '<':
					case '>':
					case '=': Relational_Op(line, &index, lineNo); break; // 500 // scan relational operators
                    case '(':
					case ')':
                    case '[':
					case ']':
					case '{':
					case '}':
					case ',': Delimiter(line, &index, lineNo); break; // 600	// got a delimiter
					case ';': token_list.push_back(createToken(700, ";", lineNo)); index++;  break;
					case ':': Assignment_Op(line, &index, lineNo); break; // 800 // scan for assignment operator

					default : throw_error(line[index], lineNo); index++; break;
				}
			}
		}

	}

	// Writing the token list on o/p file
	for(token tok : token_list) {
		file_out << "Token " << tok.token_no << ", string \"" << tok.lexeme << "\", line number " << tok.lineNo << "\n";
	}

    file_in.close();
    file_out.close();

    return 0;
}
