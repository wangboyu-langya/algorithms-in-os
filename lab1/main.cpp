// C++ implementation to read
// file word by word
#include <fstream>
#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <unordered_map>
#include <regex>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

bool IFDEBUG = false;
const int machine_size = 512;
int l = 1; // line number
int o = 0; // line offset
int len = 0; // length of the token
char c = '\0', pc = '\0';
int num_sym = 0; // number of symbols
int num_ins = 0; // number of instructions
int module = 0; // current module
vector<int> module_offset = {0}; // offset per module
vector<int> module_ins; // ins per module
int offset = 0;
int word_count = 0; // number of the word
std::ifstream ifs; // the input file
vector<vector<pair<string, bool>>> use_lists;

class Symbol {
public:
    string name; // name of the symbol
    int line_num;
    int line_off;
    int module; // module number
    int original; // original address
    int value; // final address
    int error; // error code
    bool used = false;

    Symbol(string n, int ln, int lo, int m, int o, int v, int e = 100) {
        name = n;
        line_num = ln;
        line_off = lo;
        module = m;
        original = o;
        value = v;
        error = e;
    }
};

void print_symbol(Symbol &s) {
    cout << s.name << "\t" << s.module << "\t"
         << s.original << "\t" << s.value << "\t"
         << s.line_num << "\t" << s.line_off << endl;
}

class Instruction {
public:
    int operation; // number between 0-9999
    string type; // 0123 for IARE(Immediate, Absolute, Relative, or External)
    string ref; // ref of vaiable for external type
    int line_num;
    int line_off;
    int module; // module number
    int value; // final address
    int error; // error code
    Instruction(int o, string t, string r, int ln, int lo, int m, int v, int e = 100) {
        operation = o;
        type = t;
        ref = r;
        line_num = ln;
        line_off = lo;
        module = m;
        value = v;
        error = e;
    }
};

// TODO: this could actually be accomadated in the class definition
void print_ins(Instruction &s) {
    cout << s.operation << "\t" << s.type << "\t"
         << s.ref << "\t" << s.module << "\t"
         << s.value << "\t" << s.line_num << "\t"
         << s.line_off << endl;
}

vector<Symbol> symbols;
vector<Instruction> instructs;
std::unordered_map<string, int> symbol_map;

bool is_space(char ch) {
    return std::isspace(static_cast<unsigned char>(ch));
}

int if_symbol(string &s) {
    int error = 100;
    string message = "NULL";
    if (s == "EOF") return 1;
    try {
        // parse error 1
        std::regex e("[a-zA-Z][a-zA-Z0-9]*", std::regex::ECMAScript);
        if (!regex_match(s, e)) error = 1;
    }
    catch (const std::regex_error &e) {
//        std::cout << "regex_error caught: " << e.what() << '\n';
        error = 1;
    }
    // parse error 3
    if (s.length() > 16) error = 3;
    return error;
}

int if_address(string &s) {
    int error = 100;
    string message = "NULL";
    try {
        // parse error 2
        std::regex e("[IEAR]", std::regex::ECMAScript);
        if (!regex_match(s, e)) error = 2;
    }
    catch (const std::regex_error &e) {
//        std::cout << "regex_error caught: " << e.what() << '\n';
        error = 2;
    }
    return error;
}

int if_number(string &s) {
    int error = 100;
    string message = "NULL";
    try {
        // parse error 0
        std::regex e("[0-9]*", std::regex::ECMAScript);
        if (!regex_match(s, e)) error = 0;
    }
    catch (const std::regex_error &e) {
        std::cout << "regex_error caught: " << e.what() << '\n';
        error = 0;
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
        if (c != -1) o++;
        // a newline
        if ((pc == '\n' | pc == 'r') && c != -1) {
            l++;
            o = 1;
        }
        // read a nonspace value
        if (!is_space(c) && c != -1) {
            // start a token
            if (pc == '\0' | is_space(pc)) len = 1;
            // continue reading
            if (pc != '\0' && !is_space(pc)) len++;
            token += c;
            // eof of space
        } else {
            // a token is finished
            if (!is_space(pc) && pc != '\0') {
                pc = c;
                return token;
            }
        }
    }
    if (c == -1) {
//        cout << "must be end of file, line " << l << endl;
        return "EOF";
    }
    if (token.empty()) {
//        cout << "must be end of file, line " << l << endl;
        return "NULL";
    }
    cout << "case not handle : " << token << " line :" << l << " offset :" << o - len << endl;
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
    exit(1);
}

string warnings(int e, string message = "") {
    string s;
    std::ostringstream out;
    switch (e) {
        case 1 :
            s = "Error: Absolute address exceeds machine size; zero used";
            break;
        case 2 :
            s = "Error: Relative address exceeds module size; zero used";
            break;
        case 3 :
            s = "Error: External address exceeds length of uselist; treated as immediate";
            break;
        case 4 :
//            s = "Error: %s is not defined; zero used";
            out << "Error: " << message << " is not defined; zero used";
            s = out.str();
            break;
        case 5 :
            s = "Error: Illegal immediate value; treated as 9999";
            break;
        case 6 :
            s = "Error: Illegal opcode; treated as 9999";
            break;
        case 7 :
            s = "Error: This variable is multiple times defined; first value used";
            break;
        default:
            s = "";
            break;
    }
    return s;
}

bool if_more_module(int &def_count) {
    string token;
    int error;
    // read the defcount
    if (ifs.good()) {
        token = read_token();
        if (token == "EOF")
            return false;
        error = if_number(token);
        if (error != 100) {
            __parseerror(error, l, o - len);
            exit(1);
            return false;
        } else {
            try {
                def_count = stoi(token);
            }
            catch (std::invalid_argument &e) {
                cout << "Invalid token: " << token << endl;
                cout << "error code: " << error << endl;
            };

            // parse error 4
            if (def_count > 16) {
                __parseerror(4, l, o - len);
                exit(1);
            }
            return true;
        }
    }
}

void read_def(int &def_count) {
    int error;
    // read the value pairs
    string t1, t2;
    int value = -1;
    for (int i = 0; i < def_count; ++i) {
        t1 = read_token();
        // expected a symbol
        if (t1 == "NULL") {
            __parseerror(1, l, o - len);
            exit(1);
        }
        error = if_symbol(t1);
        if (error != 100) {
            __parseerror(error, l, o - len);
            exit(1);
        }
        t2 = read_token();
        // expected a number
        if (t2 == "NULL") {
            __parseerror(0, l, o - len);
            exit(1);
        }
        error = if_number(t2);
        if (error != 100) {
            __parseerror(error, l, o - len);
            exit(1);
        } else
            try {
                value = stoi(t2);
            }
            catch (std::invalid_argument &e) {
                cout << "Invalid token: " << t2 << endl;
                cout << "error code: " << error << endl;
            };
        // add to the dict if the symbol has not been seen
        // else defined multiple times
        if (symbol_map.find(t1) == symbol_map.end()) {
            symbol_map[t1] = num_sym++;
            symbols.push_back(Symbol(t1, l, o - len, module, value, -1));
        } else
            // error 7
            symbols[symbol_map[t1]].error = 7;
    }
}

vector<pair<string, bool>> read_use() {
    string token;
    int error;
    int use_count;
    // read the usecount
    if (ifs.good()) {
        token = read_token();
        error = if_number(token);
        if (error != 100) {
            __parseerror(error, l, o - len);
            exit(1);
        } else
            try {
                use_count = stoi(token);
            }
            catch (std::invalid_argument &e) {
                cout << "Invalid token: " << token << endl;
                cout << "error code: " << error << endl;
            };
        // parse error 3
        if (use_count > 16) {
            __parseerror(5, l, o - len);
            exit(1);
        }
    }
    // read the value pairs
    vector<pair<string, bool>> use_list;
    for (int i = 0; i < use_count; ++i) {
        token = read_token();
        // expected a symbol
        if (token == "NULL") {
            __parseerror(1, l, o - len);
            exit(1);
        }
        error = if_symbol(token);
        if (error != 100) {
            __parseerror(error, l, o - len);
            exit(1);
        }
        // add this to the module use list
        use_list.push_back(make_pair(token, false));
    }
    // return use list
    return use_list;
}

void read_ins(vector<pair<string, bool>> use_list) {
    string token;
    int error;
    int ins_count;
    // read the ins_count
    if (ifs.good()) {
        token = read_token();
        error = if_number(token);
        if (error != 100) {
            __parseerror(error, l, o - len);
            exit(1);
        } else
            try {
                ins_count = stoi(token);
            }
            catch (std::invalid_argument &e) {
                cout << "Invalid token: " << token << endl;
                cout << "error code: " << error << endl;
            };
    }
    offset += ins_count;
    if (offset >= 512) {
        __parseerror(6, l, o - len);
        exit(1);
    }
    module_offset.push_back(offset);
    module_ins.push_back(ins_count);
    // read the instruction pairs
    string t1, t2;
    int operation = -1;
    for (int i = 0; i < ins_count; ++i) {
        // read instruction type
        t1 = read_token();
        // expected an address
        if (t1 == "NULL") {
            __parseerror(1, l, o - len);
            exit(1);
        }
        error = if_address(t1);
        if (error != 100) {
            __parseerror(error, l, o - len);
            exit(1);
        }
        // read the instruction
        t2 = read_token();
        // expected a number
        if (t2 == "NULL") {
            __parseerror(0, l, o - len);
            exit(1);
        }
        error = if_number(t2);
        if (error != 100) {
            __parseerror(error, l, o - len);
            exit(1);
        } else
            try {
                operation = stoi(t2);
            }
            catch (std::invalid_argument &e) {
                cout << "Invalid token: " << t2 << endl;
                cout << "error code: " << error << endl;
            };
        if (t1 == "E") {
            // error 3
            if (operation % 1000 >= use_list.size())
                instructs.push_back(Instruction(operation, "I", "NULL", l, o - len, module, -1, 3));
            else {
                string ref = use_list[operation % 1000].first;
                use_list[operation % 1000].second = true;
                instructs.push_back(Instruction(operation, t1, ref, l, o - len, module, -1));
            }
        } else
            instructs.push_back(Instruction(operation, t1, "NULL", l, o - len, module, -1));
    }
    use_lists.push_back(use_list);
}

void test_single_module(string &file_name) {
    ifs.open(file_name);
    if (!ifs) {
        cout << "Cannot open file.\n";
    }
    // read the definition list
    int def_count;
    if (if_more_module(def_count))
        read_def(def_count);
    for (auto &symbol : symbols) {
        print_symbol(symbol);
    }
    // read the use list
    vector<pair<string, bool>> use_list = read_use();
    // read the instructions
    read_ins(use_list);
    for (auto &ins : instructs) {
        print_ins(ins);
    }
    ifs.close();
    l = 1; // line number
    o = 1; // line offset
    len = 0; // length of the token
    c = '\0', pc = '\0';
}

void fprint_ins(int &count, Instruction &ins) {
//    if (count < 10)
//        cout << "00" << count << ": " << ins.value << " " << warnings(ins.error, ins.ref) << endl;
//    else if (count < 100)
//        cout << "0" << count << ": " << ins.value << " " << warnings(ins.error, ins.ref) << endl;
//    else
//        cout << count << ": " << ins.value << " " << warnings(ins.error, ins.ref) << endl;
    // 这个写法超棒
    cout << std::setw(3) << std::setfill('0') << count << ": " << std::setw(4) << std::setfill('0') << ins.value << " "
         << warnings(ins.error, ins.ref) << endl;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "Usage lab01.exe input" << endl;
        exit(0);
    }
    string file_name = argv[1];
    string token;

    // pass 1
    ifs.open(file_name);
    if (!ifs) {
        cout << "Cannot open file.\n";
        return 1;
    }
    int def_count;
    while (if_more_module(def_count)) {
        module++;
        // read the definition list
        read_def(def_count);
        // read the use list
        vector<pair<string, bool>> use_list = read_use();
        // read the instructions
        read_ins(use_list);
    }

    // pass 2
    // print them after all
    for (auto &symbol : symbols) {
        // error 8
        if (symbol.original > module_ins[symbol.module - 1]) {
            printf("Warning: Module %d: %s too big %d (max=%d) assume zero relative\n", symbol.module,
                   symbol.name.c_str(),
                   symbol.original, module_ins[symbol.module - 1] - 1);
            symbol.error = 8;
            symbol.value = 0;
        } else
            symbol.value = symbol.original + module_offset[symbol.module - 1];
    }
    for (auto &ins : instructs) {
        if (ins.type == "I") {
            // error 5
            if (ins.operation > 9999) {
                ins.error = 5;
                ins.value = 9999;
            } else
                ins.value = ins.operation;
        }
        if (ins.type == "A") {
            // error 1
            if (ins.operation % 1000 >= machine_size) {
                ins.error = 1;
                ins.value = ins.operation / 1000 * 1000;
            } else
                ins.value = ins.operation;
        }
        if (ins.type == "R") {
            // error 2
            if (ins.operation % 1000 > module_ins[ins.module - 1]) {
                ins.error = 2;
                ins.value = ins.operation / 1000 * 1000 + module_offset[ins.module - 1];
            } else
                ins.value = ins.operation + module_offset[ins.module - 1];
        }
        if (ins.type == "E") {
            // error 4
            // symbol is never defined
            if (symbol_map.find(ins.ref) == symbol_map.end()) {
                ins.error = 4;
                ins.value = ins.operation / 1000 * 1000;
            } else {
                ins.value = ins.operation / 1000 * 1000 + symbols[symbol_map[ins.ref]].value;
                symbols[symbol_map[ins.ref]].used = true;
            }
        }
        if (ins.value > 9999) {
            ins.error = 6;
            ins.value = 9999;
        }
    }

    // print them in required format
    // print symbol table
    cout << "Symbol Table" << endl;
    for (auto &symbol : symbols) {
        cout << symbol.name << "=" << symbol.value << " " << warnings(symbol.error) << endl;
    }

    // print memory map
    cout << endl << "Memory Map" << endl;
    int count = 0, mod = 0;
    for (auto &ins : instructs) {
        fprint_ins(count, ins);
        count++;
        if (count == module_offset[mod + 1]) {
            for (auto &p : use_lists[mod]) {
                if (!p.second)
                    printf("Warning: Module %d: %s appeared in the uselist but was not actually used\n", mod + 1,
                           p.first.c_str());
            }
            mod++;
        }
    }

    // error 9
    printf("\n");
//    bool if_used = false;
    for (auto &symbol : symbols) {
        if (!symbol.used) {
            printf("Warning: Module %d: %s was defined but never used\n", symbol.module,
                   symbol.name.c_str());
//            if_used = true;
        }
    }
//    if (if_used) printf("\n");

    ifs.close();
    return 0;
}
