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
#include "Pager.h"

using namespace std;

bool if_debug = false;
bool if_verbose = false;
Pge page_tye;
Pager *pager;
int frame_number = 0;
int num_process = 0;
list<Instruction> instructions;
vector<Pte> page_table;
vector<Process> processes;
std::ifstream ifs; // the input file
unordered_map<Ins, string> ins_format{{c, "c"},
                                      {r, "r"},
                                      {w, "w"}};
unordered_map<Frame *, Pte *> frame_table;

void init(int argc, char *argv[]) {
    if (argc != 6 && argc != 7) {
        cout << "Please use: lab03.exe -a[fsrnca] -o[OPFS] -f<num_frames> inputfile randfile" << endl;
        exit(0);
    }
    if (argc == 7)
        if_verbose = true;
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
                processes.push_back(Process(counter++));
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
    if (if_debug)
        cout << "Instructions:" << endl;
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
    // init the pager
    switch (page_tye) {
        case Fifo:
            pager = new Pager_fifo(frame_number);
            break;
        case Sec_chance:
            pager = new Pager_sec(frame_number);
            break;
    }
}

int main(int argc, char *argv[]) {
    init(argc, argv);
    Instruction *ins;
    Process *cp;
    Pte *cpte;
    long int counter = 0;
    while (!instructions.empty()) {
        ins = &instructions.front();
        // choose current process
        if (ins->type == c) {
            cout << counter << ": ==> c " << ins->location << endl;
            cp = &processes.at(ins->location);
            instructions.pop_front();
            counter++;
            continue;
        } else {
            // the only difference between r and w is the modified bit
            if (ins->type == w) {
                if (if_verbose)
                    cout << counter << ": ==> w " << ins->location << " " << cp->pid << endl;
                else
                    cout << counter << ": ==> w " << ins->location << endl;
            } else {
                if (if_verbose)
                    cout << counter << ": ==> r " << ins->location << " " << cp->pid << endl;
                else
                    cout << counter << ": ==> r " << ins->location << endl;
            }

            // if part of vma, access the pte
            if (cp->legal(ins->location)) {
                cpte = &cp->page_table[ins->location];
                // else throw exception
            } else {
                cout << "  SEGV" << endl;
                instructions.pop_front();
                counter++;
                continue;
            };

            // if the virtual page is valid, do nothing
            // else find the vitim
            if (counter == 149)
                cout << "stop" << endl;
            if (!cpte->valid) {
                Frame *victim = pager->get();
                // the frame is not free
                if (victim->process != -1) {
                    cout << " UNMAP " << victim->process << ":" << victim->virtual_page << endl;
                    Pte *pte_victim = &processes.at(victim->process).page_table[victim->virtual_page];
                    // page out if modified
                    if (pte_victim->modified == 1) {
                        if (pte_victim->file_map == 1)
                            cout << " FOUT" << endl;
                        else
                            cout << " OUT" << endl;
                        pte_victim->page_out = 1;
                    }
                    pte_victim->reset();
                }
                // decide the content of current pte
                if (cpte->file_map)
                    cout << " FIN" << endl;
                else if (cpte->page_out)
                    cout << " IN" << endl;
                else
                    cout << " ZERO" << endl;
                // map
                cout << " MAP " << victim->number << endl;
                // reset frame
                victim->process = cp->pid;
                victim->virtual_page = ins->location;
                victim->referenced = 0;
                victim->modified = 0;
                // reset current pte
                cpte->frame = victim->number;
                cpte->valid = 1;
            }
        }
        // reset current pte
        if (ins->type == w)
            if (cpte->write_protect == 0) {
                cpte->modified = 1;
                if (page_tye != Nru)
                    pager->frames.at(cpte->frame).referenced = 1;
            } else {
                if (page_tye != Nru)
                    pager->frames.at(cpte->frame).referenced = 1;
                cout << " SEGPROT" << endl;
            }
        else if (ins->type == r) {
            cpte->referenced = 1;
            pager->frames.at(cpte->frame).referenced = 1;
        }
        instructions.pop_front();
        counter++;
    }
}
