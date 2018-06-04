// C++ implementation to read
// file word by word
#include <fstream>
#include <iostream>
#include <string>
#include <cctype>

using namespace std;

bool IFDEBUG = true;
int l = 1;
int o = 1;
char c, pc = '\0';

bool is_space(char ch) {
    return std::isspace(static_cast<unsigned char>(ch));
}

string read_next(std::ifstream &ifs) {
    string token;
    int len = 0;
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
                if (IFDEBUG) cout << token << " line :" << l << " offset :" << o - len + 1 << endl;
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
            "NUM_EXPECTED", // Number expect
            "SYM_EXPECTED", // Symbol Expected
            "ADDR_EXPECTED", // Addressing Expected which is A/E/I/R
            "SYM_TOO_LONG", // Symbol Name is too long
            "TOO_MANY_DEF_IN_MODULE", // > 16
            "TOO_MANY_USE_IN_MODULE", // > 16
            "TOO_MANY_INSTR", // total num_instr exceeds memory size (512)
    };
    printf("Parse Error line %d offset %d: %s\n", linenum, lineoffset, errstr[errcode]);
}

// driver code
int main() {
    std::ifstream ifs;
    ifs.open("lab1samples/input-2");
    if (!ifs) {
        cout << "Cannot open file.\n";
        return 1;
    }

    string token;
    while (ifs.good()) {
        token = read_next(ifs);
    }
    ifs.close();
    return 0;
}
