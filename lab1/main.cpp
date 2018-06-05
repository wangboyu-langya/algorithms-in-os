// C++ implementation to read
// file word by word
#include <fstream>
#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <unordered_map>
#include <regex>

using namespace std;

bool IFDEBUG = false;
const int word_limit = 512;
int l = 1; // line number
int o = 1; // line offset
int len = 0; // length of the token
char c, pc = '\0';
int num_sym = 0; // number of symbols
int num_ins = 0; // number of instructions
int module = 0; // current module
int word_count = 0; // number of the word
std::ifstream ifs; // the input file

class Symbol {
public:
    string name; // name of the symbol
    int line_num;
    int line_off;
    int module; // module number
    int original; // original address
    int value; // final address
    Symbol(string n, int ln, int lo, int m, int o, int v) {
        name = n;
        line_num = ln;
        line_off = lo;
        module = m;
        original = o;
        value = v;
    }
};

void print_symbol(Symbol &s) {
    cout << s.name << "\t" << s.line_num << "\t"
         << s.line_off << "\t" << s.module << "\t"
         << s.original << "\t" << s.value << endl;
}

class Instruction {
public:
    int operation; // number between 0-9999
    int type; // 0123 for IARE(Immediate, Absolute, Relative, or External)
    int line_num;
    int line_off;
    int module; // module number
    int value; // final address
    Instruction(int o, int t, int ln, int lo, int m, int v) {
        operation = o;
        type = t;
        line_num = ln;
        line_off = lo;
        module = m;
        value = v;
    }
};

vector<Symbol> symbols;
vector<Instruction> instructs;
vector<int> module_offset;
std::unordered_map<string, int> symbol_map;

bool is_space(char ch) {
    return std::isspace(static_cast<unsigned char>(ch));
}

int if_symbol(string &s) {
    int error = 100;
    string message = "NULL";
    if (s.length() > 16) error = 3;
    try {
        std::regex e("[a-zA-Z][a-zA-Z0-9]*", std::regex::ECMAScript);
        if (!regex_match(s, e)) error = 1;
    }
    catch (const std::regex_error& e) {
        std::cout << "regex_error caught: " << e.what() << '\n';
    }
    return error;
}

int if_address(string &s) {
    int error = 100;
    string message = "NULL";
    try {
        std::regex e("[IEAR]", std::regex::ECMAScript);
        if (!regex_match(s, e)) error = 2;
    }
    catch (const std::regex_error& e) {
        std::cout << "regex_error caught: " << e.what() << '\n';
    }
    return error;
}

int if_number(string &s) {
    int error = 100;
    string message = "NULL";
    try {
        std::regex e("[0-9]{1,4}", std::regex::ECMAScript);
        if (!regex_match(s, e)) error = 0;
    }
    catch (const std::regex_error& e) {
        std::cout << "regex_error caught: " << e.what() << '\n';
    }
    return error;
}


string read_token() {
    string token;
    len = 0;
    token.clear();
    while (ifs.good()) {
        pc = c;
        c = ifs.get();
        o++;
        // a newline
        if (pc == '\n' | pc == 'r' | pc == '\0') {
            l++;
            o = 0;
        }
        if (!is_space(c)) {
            // start a token
            if (pc == '\0' | is_space(pc)) len = 1;
            // continue reading
            if (pc != '\0' && !is_space(pc)) len++;
            token += c;
        } else {
            // a token is finished
            if (!is_space(pc) && pc != '\0') {
                pc = c;
                return token;
            }
        }
    }
    if (token.empty()) {
        cout << "must be end of file, line " << l << endl;
        return "NULL";
    }
    cout << "case not handle : " << token << " line :" << l << " offset :" << o - len + 1 << endl;
    return "NULL";
}

void __parseerror(int errcode, int linenum, int lineoffset) {
    static char *errstr[] = {
            "NUM_EXPECTED", // Number expect, 0
            "SYM_EXPECTED", // Symbol Expected, 1
            "ADDR_EXPECTED", // Addressing Expected which is A/E/I/R, 2
            "SYM_TOO_LONG", // Symbol Name is too long, 3
            "TOO_MANY_DEF_IN_MODULE", // > 16, 4
            "TOO_MANY_USE_IN_MODULE", // > 16, 5
            "TOO_MANY_INSTR", // total num_instr exceeds memory size (512), 6
    };
    printf("Parse Error line %d offset %d: %s\n", linenum, lineoffset, errstr[errcode]);
}

void read_def() {
    string token;
    int error;
    int def_count;
    // read the defcount
    if (ifs.good()) {
        token = read_token();
        error = if_number(token);
        if (error != 100)
            __parseerror(error, l, o - len + 1);
        else
            def_count = stoi(token);
    }
    // read the value pairs
    string t1, t2;
    int value = -1;
    for (int i = 0; i < def_count; ++i) {
        t1 = read_token();
        // expected a symbol
        if (t1 == "NULL")
            __parseerror(1, l, o - len + 1);
        error = if_symbol(t1);
        if (error != 100)
            __parseerror(error, l, o - len + 1);
            // add to the dict if the symbol has not been seen
        else if (symbol_map.find(token) != symbol_map.end())
            symbol_map[token] = ++num_sym;
        t2 = read_token();
        // expected a number
        if (t2 == "NULL")
            __parseerror(0, l, o - len + 1);
        error = if_number(t2);
        if (error != 100)
            __parseerror(error, l, o - len + 1);
        else
            value = stoi(t2);
        symbols.push_back(Symbol(t1, l, o - len + 1, module, value, -1));
    }
}


// driver code
int main() {
    string file_name = "lab1samples/input-2";
    ifs.open(file_name);
    if (!ifs) {
        cout << "Cannot open file.\n";
        return 1;
    }

    string token;
    // this can test read_token
    if (IFDEBUG)
        while (ifs.good()) {
            token = read_token();
            if (token != "NULL")
                if (IFDEBUG) cout << token << " line :" << l << " offset :" << o - len + 1 << endl;
        }
    ifs.close();
    l = 1; // line number
    o = 1; // line offset
    len = 0; // length of the token

    // pass 1
    ifs.open(file_name);
    if (!ifs) {
        cout << "Cannot open file.\n";
        return 1;
    }
    read_def();
    for (auto &symbol : symbols) {
        print_symbol(symbol);
    }
    ifs.close();
    return 0;
}
