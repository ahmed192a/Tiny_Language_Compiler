#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <iterator>
#include <sstream>


using namespace std;

void find_first(vector< pair<string, vector<string>> > gram, 
	map< string, set<string> > &firsts, 
	string non_term); 

void find_follow(vector< pair<string, vector<string>> > gram, 
	map< string, set<string> > &follows, 
	map< string, set<string> > firsts, 
	string non_term); 


int main(int argc, char const *argv[])
{
	if(argc != 3) {
		cout<<"Arguments should be <grammar file> <input string>\n";
		return 1;
	}
	// Arguments check
	// cout<<argv[1]<<argv[2];

	
	// Parsing the grammar file
	fstream grammar_file;
	grammar_file.open(argv[1], ios::in);
	if(grammar_file.fail()) {
		cout<<"Error in opening grammar file\n";
		return 2;
	}

	cout<<"Grammar parsed from grammar file: \n";
	vector< pair<string, vector<std::string>> > gram;
	int count = 0;
	string buffer="";
	while(std::getline(grammar_file, buffer)) {
		//char buffer[20];
		
		//grammar_file.getline(buffer, 19);


		if(buffer=="")break;

		string lhs = buffer.substr(0, buffer.find('-'));
		string rhs = buffer.substr(buffer.find('>')+1,buffer.length()-(buffer.find('>')+1));
		
		std::stringstream ss(rhs);
		istream_iterator<std::string> begin(ss);
		istream_iterator<std::string> end;
		vector<std::string> vstrings(begin, end);
		
		pair <string, vector<std::string>> prod (lhs, vstrings);
		gram.push_back(prod);
		cout<<count++<<".  "<<gram.back().first<<" -> "<<rhs<<"\n";
	}
	cout<<"\n";


	// Gather all non terminals
	set<string> non_terms;
	for(auto i = gram.begin(); i != gram.end(); ++i) {
		non_terms.insert(i->first);
	}
	cout<<"The non terminals in the grammar are: ";
	for(auto i = non_terms.begin(); i != non_terms.end(); ++i) {
		cout<<*i<<" ";
	}
	cout<<"\n";
	// Gather all terminals
	set<string> terms;
	for(auto i = gram.begin(); i != gram.end(); ++i) {

		//for(auto kl = vstrings.begin(); kl != vstrings.end();kl++) cout<<*kl<<endl;
		//copy(vstrings.begin(), vstrings.end(), ostream_iterator<std::string>(std::cout, " "));
		
		
		for(auto ch = i->second.begin(); ch != i->second.end(); ++ch) {
		
			//(*ch)[0]=='t'? (*ch)[0]=='_'?:cout<<(*ch)[0] <<"\n";
			if((*ch)[0]=='t' &&(*ch)[1]=='_') {

				//string t = (*ch).substr(2, (*ch).length());
				string t = (*ch);
				// Remove epsilon and add end character $
				if(t=="t_e") t = "$";
				terms.insert(t);
			}
		}
		
	}
	


	cout<<"The terminals in the grammar are: ";
	for(auto i = terms.begin(); i != terms.end(); ++i) {
		cout<<*i<<" ";
	}
	cout<<"\n\n";

	
	// Start symbol is first non terminal production in grammar
	string start_sym = gram.begin()->first;


	map< string, set<string> > firsts;
	for(auto non_term = non_terms.begin(); non_term != non_terms.end(); ++non_term) {
		if(firsts[*non_term].empty()){
			find_first(gram, firsts, *non_term);
		}
	}

	cout<<"Firsts list: \n";
	for(auto it = firsts.begin(); it != firsts.end(); ++it) {
		cout<<it->first<<" : ";
		for(auto firsts_it = it->second.begin(); firsts_it != it->second.end(); ++firsts_it) {
			cout<<*firsts_it<<" ";
		}
		cout<<"\n";
	}
	cout<<"\n";


	map< string, set<string> > follows;
	// Find follow of start variable first
	string start_var = gram.begin()->first;
	follows[start_var].insert("$");
	find_follow(gram, follows, firsts, start_var);
	// Find follows for rest of variables
	for(auto it = non_terms.begin(); it != non_terms.end(); ++it) {
		if(follows[*it].empty()) {
			find_follow(gram, follows, firsts, *it);
		}
	}

	cout<<"Follows list: \n";
	for(auto it = follows.begin(); it != follows.end(); ++it) {
		cout<<it->first<<" : ";
		for(auto follows_it = it->second.begin(); follows_it != it->second.end(); ++follows_it) {
			cout<<*follows_it<<" ";
		}
		cout<<"\n";
	}
	cout<<"\n";


	int parse_table[non_terms.size()][terms.size()];
	fill(&parse_table[0][0], &parse_table[0][0] + sizeof(parse_table)/sizeof(parse_table[0][0]), -1);
	
	for(auto prod = gram.begin(); prod != gram.end(); ++prod) {
		vector<string> rhs = prod->second;

		set<string> next_list;
		bool finished = false;
		for(auto ch = rhs.begin(); ch != rhs.end(); ++ch) {
			if((*ch)[0]=='t' &&(*ch)[1]=='_') {
				if(*ch != "t_e") {
					next_list.insert(*ch);
					finished = true;
					break;
				}
				continue;
			}

			set<string> firsts_copy(firsts[*ch].begin(), firsts[*ch].end());
			if(firsts_copy.find("t_e") == firsts_copy.end()) {
				next_list.insert(firsts_copy.begin(), firsts_copy.end());
				finished = true;
				break;
			}
			firsts_copy.erase("t_e");
			next_list.insert(firsts_copy.begin(), firsts_copy.end());
		}
		// If the whole rhs can be skipped through epsilon or reaching the end
		// Add follow to next list
		if(!finished) {
			next_list.insert(follows[prod->first].begin(), follows[prod->first].end());
		}


		for(auto ch = next_list.begin(); ch != next_list.end(); ++ch) {
			int row = distance(non_terms.begin(), non_terms.find(prod->first));
			int col = distance(terms.begin(), terms.find(*ch));
			int prod_num = distance(gram.begin(), prod);
			if(parse_table[row][col] != -1) {
				cout<<"Collision at ["<<row<<"]["<<col<<"] for production "<<prod_num<<"\n";
				continue;
			}
			parse_table[row][col] = prod_num;
		}

	}
	
	
	// Print parse table
	cout<<"Parsing Table: \n\t\t";
	cout<<"   ";
	for(auto i = terms.begin(); i != terms.end(); ++i) {
		cout<<*i<<"\t\t";
	}
	cout<<"\n";
	
	for(auto row = non_terms.begin(); row != non_terms.end(); ++row) {
		cout<<*row<<" \t\t";
		for(int col = 0; col < terms.size(); ++col) {
			int row_num = distance(non_terms.begin(), row);
			if(parse_table[row_num][col] == -1) {
				cout<<"-\t\t";
				continue;
			}
			cout<<parse_table[row_num][col]<<"\t\t";
		}
		cout<<"\n";
	}
	cout<<"\n";


	//string input_string_r(argv[2]);
	string input_string_r;

	// Parsing the grammar file
	fstream  input_file;
	input_file.open(argv[2], ios::in);
	if(input_file.fail()) {
		cout<<"Error in opening input file\n";
		return 5;
	}
	if(!std::getline( input_file, input_string_r)){
		cout<<"Error input file is empty\n";
		return 6;
	}


	std::vector<std::string> v_input_string;
	if( input_string_r.find('-')!= string::npos){
		std::stringstream test(input_string_r);

		std::string segment;

		while(std::getline(test, segment, '-'))
		{
		   v_input_string.push_back(segment);
		}
	}else{
		v_input_string.push_back(input_string_r);
	}

	v_input_string.push_back("$");
	stack<string> st;
	st.push("$");
	st.push(gram.begin()->first);



	// Check if input string is valid
	for(auto ch = v_input_string.begin(); ch != v_input_string.end(); ++ch) {
		if((*ch)!="$" &&terms.find(*ch) == terms.end()) {
			cout<<"Input string is invalid " << *ch<<"\n";
			return 2;
		}
	}
	cout<<"Parsing Actions : \n";
	// cout<<"Processing input string\n";
	bool accepted = true;
	while(!st.empty() && !v_input_string.empty()) {

		// If stack top same as input string char remove it

		if(v_input_string[0] == st.top()) {
			st.pop();
			v_input_string.erase(v_input_string.begin());
		}
		else if(st.top()[0]=='t' && st.top()[1]=='_') {
			cout<<"Unmatched terminal found\n";
			accepted = false;
			break;
		}
		else {
			string stack_top = st.top();
			int row = distance(non_terms.begin(), non_terms.find(stack_top));
			int col = distance(terms.begin(), terms.find(v_input_string[0]));
			int prod_num = parse_table[row][col];

			if(prod_num == -1) {
				cout<<stack_top;
				cout<<"  "<<v_input_string[0]<<endl;
				cout<<"No production found in parse table\n";
				accepted = false;
				break;
			}
			st.pop();
			vector<string> rhs = gram[prod_num].second;
			if(rhs[0] == "t_e") {

				continue;
			}

			cout<<"\t"<<gram[prod_num].first <<" -> ";
			for(auto ch = rhs.rbegin(); ch != rhs.rend(); ++ch) {
				cout<<*ch<<" ";
				st.push(*ch);
			}
			cout<<endl;
		}
	}
	cout<<endl;
	if(accepted) {
		cout<<"Input string is accepted\n";
	}
	else {
		cout<<"Input string is rejected\n";
	}

	return 0;
}

void find_first(vector< pair<string, vector<string>> > gram, 
	map< string, set<string> > &firsts, 
	string non_term) {

	//cout<<"Finding firsts of "<<non_term<<"\n";

	for(auto it = gram.begin(); it != gram.end(); ++it) {
		// Find productions of the non terminal
		if(it->first != non_term) {
			continue;
		}

		//cout<<"Processing production "<<it->first<<"\n";

		vector<string> rhs = it->second;
		// Loop till a non terminal or no epsilon variable found
		for(auto ch = rhs.begin(); ch != rhs.end(); ++ch) {
			// If first char in production a non term, add it to firsts list
			if((*ch)[0]=='t' &&(*ch)[1]=='_') {
				firsts[non_term].insert(*ch);
				break;
			}
			else {
				// If char in prod is non terminal and whose firsts has no yet been found out
				// Find first for that non terminal
				if(firsts[*ch].empty()) {
					find_first(gram, firsts, *ch);
				}
				// If variable doesn't have epsilon, stop loop
				if(firsts[*ch].find("t_e") == firsts[*ch].end()) {
					firsts[non_term].insert(firsts[*ch].begin(), firsts[*ch].end());
					break;
				}

				set<string> firsts_copy(firsts[*ch].begin(), firsts[*ch].end());

				// Remove epsilon from firsts if not the last variable
				if(ch + 1 != rhs.end()) {
					firsts_copy.erase("t_e");
				}

				// Append firsts of that variable
				firsts[non_term].insert(firsts_copy.begin(), firsts_copy.end());
			}
		}
		
	}
}

void find_follow(vector< pair<string, vector<string>> > gram, 
	map< string, set<string> > &follows, 
	map< string, set<string> > firsts, 
	string non_term) {

	// cout<<"Finding follow of "<<non_term<<"\n";

	for(auto it = gram.begin(); it != gram.end(); ++it) {

		// finished is true when finding follow from this production is complete
		bool finished = true;
		auto ch = it->second.begin();

		// Skip variables till reqd non terminal
		for(;ch != it->second.end() ; ++ch) {
			if(*ch == non_term) {
				finished = false;
				break;
			}
		}
		++ch;

		for(;ch != it->second.end() && !finished; ++ch) {
			// If non terminal, just append to follow
			if((*ch)[0]=='t' &&(*ch)[1]=='_') {
				follows[non_term].insert(*ch);
				finished = true;
				break;
			}

			set<string> firsts_copy(firsts[*ch]);
			// If char's firsts doesnt have epsilon follow search is over 
			if(firsts_copy.find("t_e") == firsts_copy.end()) {
				follows[non_term].insert(firsts_copy.begin(), firsts_copy.end());
				finished = true;
				break;
			}
			// Else next char has to be checked after appending firsts to follow
			firsts_copy.erase("t_e");
			follows[non_term].insert(firsts_copy.begin(), firsts_copy.end());

		}


		// If end of production, follow same as follow of variable
		if(ch == it->second.end() && !finished) {
			// Find follow if it doesn't have
			if(follows[it->first].empty()) {
				find_follow(gram, follows, firsts, it->first);
			}
			follows[non_term].insert(follows[it->first].begin(), follows[it->first].end());
		}

	}

}
