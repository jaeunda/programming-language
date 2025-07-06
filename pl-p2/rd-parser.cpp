#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <unordered_map>

using namespace std;

/* GLOBAL VARIABLES*/
string input_line;
istringstream iss;
string next_token;
unordered_map<string, long long> symbol_table;
ostringstream output_buffer;

/* FUNCTIONS */
int program();
int declaration();
int statement(int exec_flag);
int bexpr(long long& result);
int aexpr(long long& result);
int term(long long& result);
int factor(long long& result);
int number(long long& result);
int var();

/* UTIL FUNCTIONS */
void prompt();
int get_line();
void print_error();
void flush_buffer();
int lexeme();

/* PROGRAM */
#define TRUE 1
#define FALSE 0

int main() {
    while (program());
    exit(0);
}
int program() { /* <program> -> {<declaration>} {<statement>} */
    // prompt
    prompt();
    if (!get_line()) return FALSE;

    int declaration_loop = 1;
    int statement_loop = 1;

    // declaration
    while (!(declaration_loop = declaration()));

    if (declaration_loop < 0){ // syntax error
        output_buffer.clear();
        symbol_table.clear();
        print_error();
        return TRUE;
    }
    if (iss.eof()){ // EOF flag
        symbol_table.clear();
        iss.clear();
        return TRUE;
    }

    // statement
    while (!(statement_loop = statement(TRUE)));
    if (statement_loop < 0){ // syntax error
        output_buffer.clear();
        print_error();
    }

    // init
    flush_buffer();
    symbol_table.clear();

    return TRUE;
}
int declaration(){ /* <type> <var> */
    // RETURN VALUE
    //   0: declaration
    //   1: statement or EOF
    //  -1: syntax error

    string var_name;
   
   	// integer
    if (next_token == "integer"){
        if (lexeme())
            return -1;
        if (var())
            return -1;
        else
            var_name = next_token;
        if (lexeme())
            return -1;
        if (next_token == ";"){
            symbol_table[var_name] = 0;
        } else return -1;
	// statement
    } else if (next_token == "print" || next_token == "while" || next_token == "if") { 
        return 1;
    } else if (!var()){
        if (!symbol_table.count(next_token)) // syntax error
            return -1;
        else 
            return 1;
    } else
        return -1;

    lexeme();
    return 0;
}
int statement(int exec_flag) { 
    // RETURN VALUE
    //     0: statement
    //     1: EOF
    //    -1: syntax error

    /* <statement> 
        -> <var> = <aexpr> ; | print <aexpr> ; |
            while '(' <bexpr> ')' do '{' {<statement>} '}' ; |
            if '(' <bexpr> ')' '{' {<statement>} '}' else '{' {<statement>} '}' ;   
    */

    long long aexpr_res = 0, bexpr_res = TRUE;

	// PRINT
    if (next_token == "print"){
        if (lexeme())
            return -1;
        if (aexpr(aexpr_res))
            return -1;
        if (next_token != ";")
            return -1;
        if (exec_flag)
            output_buffer << aexpr_res << " ";
    }
	// WHILE
	else if (next_token == "while"){
        streampos loop_pos;
        if (lexeme())
            return -1;
        if (next_token != "(")
            return -1;
        if (exec_flag)
			loop_pos = iss.tellg();
        if (lexeme())
            return -1;
        while (bexpr_res){
            if (bexpr(bexpr_res))
                return -1;
            if (next_token != ")")
                return -1;
            if (lexeme())
                return -1;
            if (next_token != "do")
                return -1;
            if (lexeme())
                return -1;
            if (next_token != "{")
                return -1;
            if (lexeme())
                return -1;
			int while_flag = exec_flag && bexpr_res ? TRUE : FALSE;
            while (next_token != "}"){
                if (statement(while_flag) < 0)
                    return -1;
            }
            if (while_flag){
                iss.clear();
                iss.seekg(loop_pos);
				if (lexeme())
					return -1;
            } else {
				if (lexeme())
					return -1;
				if (next_token != ";")
					return -1;
				break;
			}
        }

    } 
	// IF
	else if (next_token == "if"){
        if (lexeme())
            return -1;
        if (next_token != "(")
            return -1;
        if (lexeme())
            return -1;
        if (bexpr(bexpr_res))
            return -1;
        if (next_token != ")")
            return -1;
        if (lexeme())
            return -1;
        if (next_token != "{")
            return -1;
        if (lexeme())
            return -1;
        // if-else
		int if_flag = exec_flag && bexpr_res ? TRUE : FALSE;
		int else_flag = exec_flag && !bexpr_res ? TRUE : FALSE;
        while (next_token != "}"){
            if (statement(if_flag) < 0)
                return -1;
        }
        if (lexeme())
            return -1;
        if (next_token != "else")
            return -1;
        if (lexeme())
            return -1;
        if (next_token != "{")
            return -1;
        if (lexeme())
            return -1;
        while (next_token != "}"){
            if (statement(else_flag) < 0)
                return -1;
        }
        if (lexeme())
            return -1;
        if (next_token != ";")
            return -1;
    } 
	// ASSIGN
	else if (!var()){
        string var_name = next_token;
        if (!symbol_table.count(var_name))
            return -1;
        if (lexeme())
            return -1;
        if (next_token != "=")
            return -1;
        if (lexeme())
            return -1;
        if (aexpr(aexpr_res))
            return -1;
        if (next_token != ";")
            return -1;
        if (exec_flag)
            symbol_table[var_name] = aexpr_res;
    } else return -1;

    if (lexeme()){
        if (iss.eof()){
            iss.clear();
            return 1;
        }
        return -1;
    }
    return 0;
}
int bexpr(long long &result){ /* <var> <relop> <var> */
    // RETURN VALUE
    //   0: success
    //  -1: error
    long long op_lhs, op_rhs;
    string op;
    result = -1;

    // LHS
    if (!var()){
        string var_name = next_token;
        if (!symbol_table.count(var_name))
            return -1;
        else  
            op_lhs = symbol_table[var_name];
    } else
        return -1;

    if (lexeme())
        return -1;

    // relational operation
    op = next_token;
    if (lexeme())
        return -1;
    
    // RHS
    if (!var()){
        string var_name = next_token;
        if (!symbol_table.count(var_name))
            return -1;
        else
            op_rhs = symbol_table[var_name];
    } else 
        return -1;

    /* OPERATION */
    if (op == "=="){
        if (op_lhs == op_rhs) result = TRUE;
        else result = FALSE;
    } else if (op == "!="){
        if (op_lhs != op_rhs) result = TRUE;
        else result = FALSE;
    } else if (op == "<"){
        if (op_lhs < op_rhs) result = TRUE;
        else result = FALSE;
    } else if (op == ">"){
        if (op_lhs > op_rhs) result = TRUE;
        else result = FALSE;
    } else 
        return -1;

    if (result < 0) 
        return -1;
    if (lexeme())
        return -1;
    return 0;
}
int aexpr(long long& result) { /* <aexpr> -> <term> { ( + | - ) <term> } */
    // RETURN VALUE
    //     0: success
    //    -1: error
    if (term(result)) {
        return -1;
    } 
    if (next_token == ";"){
        return 0;
    }
    
    /* OPERATION */
    while (next_token == "+" || next_token == "-") {
        string op = next_token;
        if (lexeme()) {
            return -1;
        }

        long long op_rhs;
        if (term(op_rhs)) {
            return -1;
        }
        if (op == "+")  
            result += op_rhs;
        else if (op == "-") 
            result -= op_rhs;
    }
    return 0;
}
int term(long long& result) { /* <term> -> <factor> { * <factor> } */
    // RETURN VALUE
    //     0: success
    //    -1: error
    if (factor(result)) {
        return -1;
    }
    if (next_token == ";"){
        return 0;
    }
    while (next_token == "*") {
        if (lexeme()) {
            return -1;
        }
        long long op_rhs;
        if (factor(op_rhs)) 
            return -1;
        else 
            result *= op_rhs;
    }
    return 0;
}
int factor(long long& result) { /* <factor> -> [-] ( <number> | <var> | '(' <aexpr> ')' ) */
    // RETURN VALUE
    //     0: success
    //    -1: error
    bool negative = false;

    if (next_token == "-") {
        negative = true;
        if (lexeme()){
            return -1;
        }
    }
    if (next_token == "(") {
        if (lexeme()) {
            return -1;
        }
        if (aexpr(result)) {
            return -1;
        }
        if (next_token == ")") {
            if (lexeme()) {
                return -1;
            }
        } else return -1;
    } else if (!number(result)) {
        if (lexeme()){
            return -1;
        }
    } else if (!var()){
        string var_name = next_token;
        if (!symbol_table.count(var_name)) 
            return -1;
        else 
            result = symbol_table[var_name];
        if (lexeme()){
            return -1;
        }
    } else return -1;

    if (negative)
        result = -result;

    return 0;
}
int number(long long& result) { /* <number> -> <digit> { <digit> } */
    // RETURN VALUE
    //     0: success
    //    -1: error
    for (char c: next_token){
        /* <digit> -> 0 | 1 | ... | 9 */
        if (!isdigit(c)){
            return -1;
        }
    }
    result = stoll(next_token);
    return 0;
}
int var(){ /* <var> -> <alphabet> { <alphabet> } */
    // RETURN VALUE
    //     0: success
    //    -1: error
    for (char c: next_token){
        /* <alphabet> -> a | b | ... | z */
        if (!islower(c)){
            return -1;
        }
    }
    return 0;
}
/* UTIL FUNCTIONS */
void prompt(){
    cout << ">> " ;
}
int get_line(){
    // RETURN VALUE
    //   TRUE: success
    //  FALSE: 개행 문자만 입력 or 에러
    if (!getline(cin, input_line)){
        return FALSE;
    }
    if (input_line.empty()){
        return FALSE;
    }
    iss = istringstream(input_line);
    if (lexeme() < 0){
        return FALSE;
    }
    return TRUE;
}
void print_error(){
    output_buffer.str("");
    output_buffer.clear();
    cerr << "Syntax Error!" << endl;
}
void flush_buffer(){
    if (!output_buffer.str().empty()){
        cout << output_buffer.str() << endl;
    }
    output_buffer.str("");
    output_buffer.clear();
}
int lexeme(){
    // RETURN VALUE
    //     0: success
    //    -1: error
    if (!(iss >> next_token)){
        // end of line
        return -1;
    }
    return 0;
}
