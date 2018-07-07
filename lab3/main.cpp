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
#include <cstdlib>
#include <cstdio>
#include <list>
#include "utils.h"
#include "Process.h"

using namespace std;

bool if_debug = true;
Pge page_tye;
int frame_number = 0;
int num_process = 0;
vector<Instruction> instructions;
vector<Pte> page_table;
vector<Process> processes;
std::ifstream ifs; // the input file

void init(int argc, char *argv[]) {
    if (argc != 6) {
        cout << "Please use: lab03.exe -a[fsrnca] -o[OPFS] -f<num_frames> inputfile randfile" << endl;
        exit(0);
    }
    string input_name, rand_name, num_frames, pager_name;
    pager_name = argv[1];
    num_frames = argv[3];
    frame_number = stoi(num_frames.substr(2, string::npos));
    input_name = argv[4];
    rand_name = argv[5];
    unordered_map<char, Pge> type_map{
            {'f', Fifo},
            {'s', Sec_chance},
            {'r', Random},
            {'n', Nru},
            {'c', Clock},
            {'a', Aging}
    };
    page_tye = type_map[pager_name[2]];
    // load random file
    load_rand(rand_name);
    // read all the process into an vector
    ifs.open(input_name);
    if (!ifs) {
        cout << "Cannot open input file.\n";
        exit(1);
    }
    // temporary variables to read processes
    string l;
    // get the number of processes
    while (getline(ifs, l)) {
        // skip the beginning of comment
        if (l[0] == '#')
            continue;
            // get the number and exit
        else {
            istringstream iss(l);
            string token;
            iss >> token;
            num_process = stoi(token);
            if (if_debug)
                cout << "num of process: " << num_process << endl;
            break;
        }
    }
    // get the process info
    bool read_vma = false;
    int num_vma = 0;
    int counter = 0;
    Process *current;
    while (getline(ifs, l)) {
        // skip the beginning of comment
        if (l[0] == '#') {
            read_vma = false;
            num_vma = 0;
            // end the process reading
            if (counter == num_process)
                break;
            else
                continue;
        } else {
            istringstream iss(l);
            string token;
            // add a new process and increment counter
            if (!read_vma) {
                iss >> token;
                num_vma = stoi(token);
                if (if_debug)
                    cout << "Process " << counter << ": " << endl;
                Process p(counter++);
				Process tmp(p);
                current = &p;
                processes.push_back(*current);
                read_vma = true;
            }
                // else read a vma
            else {
                int a[4];
                int j = 0;
                while (iss >> token)
                    a[j++] = stoi(token);
                // insert a vma
                processes.back().insert(a[0], a[1], a[2], a[3]);
                if (if_debug)
                    cout << a[0] << " " << a[1] << " " << a[2] << " " << a[3] << endl;
                if (j != 4) {
                    cout << "Error reading vma.\n";
                    exit(1);
                }
            }
        }
    }
    // get the instructions
    unordered_map<char, Ins> ins_map{{'c', c},
                                     {'r', r},
                                     {'w', w}};
    while (getline(ifs, l)) {
        if (l[0] == '#') break;
        string token;
        istringstream iss(l);
        char t;
        int addr = 0;
        iss >> token;
        t = token[0];
        iss >> token;
        addr = stoi(token);
        if (if_debug)
            cout << t << " " << addr << endl;
        // assume all the processes are increasing arrival time order
        instructions.push_back(Instruction(ins_map[t], addr));
    }
}

int main(int argc, char *argv[]) {
    init(argc, argv);
}
