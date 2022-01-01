#include <iostream>
#include <iterator>
#include <map>
#include <fstream>
#include <string>
#include <sstream>
#include <bitset>
#include <bits/stdc++.h>
#include <cstring>
#include<ctype.h>
using namespace std;

void start_state();
void comment_state();
void id_state(fstream * input);
void assign_state();
void num_state(fstream *input);

enum states{
	start,
	in_comment,
	in_id,
	in_num,
	in_assign,
	done
};

bool flag =0;
enum states  state = start;
string	token_text = "";
string	token_type = "";
string	num_type = ""; 
char c;
string special_EXP = {'+', '-', '*', '/', '=', '<', '>', '(', ')', ';'};
map<string , string> CONST_EXP= {
    {"if"	, "IF"},
	{"then"	, "THEN"},
	{"else"	, "ELSE"},
	{"end"	, "END"},
	{"repeat", "REPEAT"},
	{"until"	, "UNTIL"},
	{"read"	, "READ"},
	{"write"	, "WRITE"},

	{"+"		, "PLUS"},
	{"-"		, "MINUS"},
	{"*"		, "MULT"},
	{"/"		, "DIV"},
	{"="		, "EQUAL"},
	{"<"		, "LESSTHAN"},
	{">"		, "GREATERTHAN"},
	{"("		, "OPENBRACKET"},
	{")"		, "CLOSEDBRACKET"},
	{";"		, "SEMICOLON"},
	{":="	, "ASSIGN"},

	{"num"	, "NUMBER"},
	{"id"	, "IDENTIFIER"}
    
};



int main()
{
	fstream input;
	ofstream output;
	input.open("test.txt", ios::in );
	output.open("out.txt", ios::out | ios::app );

	
	while(true)
	{
		if(!input.eof()){
		if(flag == 0){
			
				input.get(c);
			
		}
		else
			flag = 0;
		}else 
				c= 0x05;
		if(input.eof()){
			if (CONST_EXP[token_type]!= ""){
				cout<<"ddd : "<<c<<" f";
				output<<token_text <<", " << CONST_EXP[token_type]<<endl;
				state = start;
				token_text = "";
				token_type = "";
				num_type  = "";
			}
			//print("Finished Scanning!")
			break;
		}

		
		// States for FSM
		switch(state ){
			case start:
			start_state();
			break;
			case in_comment:
			comment_state();
			break;
			case in_id:
			id_state(&input);
			break;
			case in_num:
			num_state(&input);
			break;
			case in_assign:
			assign_state();
			break;
		}
		if(state == done){
			
			if (CONST_EXP[token_type]!= ""){
				output<<token_text <<", " << CONST_EXP[token_type]<<endl;
				state = start;
				token_text = "";
				token_type = "";
				num_type  = "";
			}
			//else:
			//	raise ScannerError('Unkown Token Type: ' + char)
		}
	}
	input.close();
	output.close();
  
    return 0;
}

void start_state(){
	if (c == '{')
		state = in_comment;
	else if (c == ':')
		state = in_assign;
	else if (isalpha(c))
	{
		state = in_id;
		token_text = c;
		token_type = "id";
	}
	else if (isdigit(c) || c == '.')
	{
		state = in_num;
		token_text = c;
		token_type = "num";
		if (c == '.')
			num_type = "fraction_part";
		else
			num_type = "integer_part";
	}
	else if (special_EXP.find(c) != std::string::npos){
		state = done;
		token_text = c;
		token_type = c;
	}
	//else
	//	raise ScannerError('Unknown Character: ' + char)
}

void comment_state(){
	if (c == '}')
		state = start;
}

void num_state(fstream *input){
	if (num_type == "integer_part"){
		if (isdigit(c))
			token_text += c;
		else if (c== '.'){
			token_text += c;
			num_type = "fraction_part";
		}
		else if (c=='e' || c=='E'){
			token_text += c;
			//# Cheat by reading the next sign of the exponent (unclean)
			*input>>c;
			if (c=='+' || c == '-')
				token_text += c;
			else{
				flag  = 1;
				num_type = "exponent_part";
			}
		}
		else{
			 flag = 1;
			state = done;
		}
	}
	else if (num_type == "fraction_part"){
		if (isdigit(c))
			token_text += c;
		else if (c=='e' || c == 'E'){
			token_text += c;
			//# Cheat by reading the next sign of the exponent (unclean)
			*input>>c;
			if (c=='+' || c == '-')
				token_text += c;
			else {
				flag =1;
				num_type = "exponent_part";
			}
		}
		else{
			flag =1;
			state = done;
		}
	}
	else if (num_type == "exponent_part"){
		if (isdigit(c))
			token_text += c;
		else{
			flag = 1;
			state = done;
		}
	}
	//else:
	//	raise ScannerError('Unknown Number State: ' + num_type)
}

void id_state(fstream * input){
	if (isalpha(c) || isdigit(c))
		token_text += c;
	else{
		//if(!(*input).eof())
		flag = 1;
		state = done;
		//# for reserved words
		if (CONST_EXP[token_text]!= "" && (token_text != "num" && token_text !="id"))
			token_type = token_text;
	}
	
}

void assign_state(){
	if (c == '='){
		state = done;
		token_text = ":=";
		token_type = ":=";
	}
	//else
	//	raise ScannerError('Unkown Operator: ' + char)
	
}