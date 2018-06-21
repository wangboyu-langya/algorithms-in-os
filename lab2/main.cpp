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

bool if_verbose; // whether verbose output
bool IF_DEBUG = true;
int quantum; // quantum for RR and PRIO only
std::ifstream ifs, rfs; // the input file
vector<int> rand_v; // random vector
// scheduler type
enum sch {
    FCFS, LCFS, SJF, RR, PRIO
};
sch sch_type;

class Process {
public:
    int pid;
    int finish; // finish time
    int turn_around; // turnaround time
    int io; // I/O time
    int prio; // priority
    int arrival; // arrival time
    int total_cpu;
    int cpu_burst;
    int io_burst;
    Process(int p, int at, int tc, int cb, int ib) {
        pid = p;
        arrival = at;
        total_cpu = tc;
        cpu_burst = cb;
        io_burst = ib;
    }
    void print() {
        cout << pid << ' ' << arrival << ' ' << total_cpu << ' ' << cpu_burst << ' ' << io_burst << endl;
    }
};
vector<Process> processes;

// init the arguments and input files
void init(int argc, char *argv[]) {
    if (argc < 5 | argc > 6) {
        cout << "Please use: lab02.exe [-v] -s [FLS|R<num>|P<num>] inputfile randfile" << endl;
        exit(0);
    }
    string input_name, rand_name, sch_name;
    // whether there is verbose output
    if (argc == 5) {
        sch_name = argv[2];
        input_name = argv[3];
        input_name = argv[4];
        if_verbose = false;
    } else {
        sch_name = argv[3];
        input_name = argv[4];
        input_name = argv[5];
        if_verbose = true;
    }
    // decide the type of scheduler
    // get the quantum if RR or PRIO
//    unordered_map<string, sch> type_map {
//            {"F", FCFS}, {"L", LCFS}, {"S", SJF}, {"R", RR}, {"P", PRIO}
//    };

    unordered_map<char, sch> type_map {
            {'F', FCFS}, {'L', LCFS}, {'S', SJF}, {'R', RR}, {'P', PRIO}
    };

    sch_type = type_map[sch_name[0]];
    if (sch_type == RR | sch_type == PRIO)
        if (sch_type == RR | sch_type == PRIO)
    // read random number into an array
    rfs.open(input_name);
    if (!rfs) {
        cout << "Cannot open random file.\n";
        exit(1);
    }
    // temporary variables to read random number
    string rand_s;
    rfs >> rand_s;
    int len = stoi(rand_s);
    while (rfs >> rand_s) {
        rand_v.push_back(stoi(rand_s));
    }
    // make sure this process is right
    if (rand_v.size() != len) {
        cerr << "Error reading random file, the number doesn't match.\n";
        exit(1);
    }
    // read all the process into an vector
    ifs.open(input_name);
    if (!ifs) {
        cout << "Cannot open input file.\n";
        exit(1);
    }
    // temporary variables to read processes
    string l;
    int i;
    while (getline(ifs, l)) {
        string token;
        istringstream iss(l);
        int a[4];
        int j;
        while (iss >> token)
            a[j++] = stoi(token);
        if (j != 4) {
            cout << "Error reading processes.\n";
            exit(1);
        }
        processes.push_back(Process(i, a[0], a[1], a[2], a[3]));
        i++;
    }
    for(Process &s : processes)
        s.print();
}

int main(int argc, char *argv[]) {
    init(argc, argv);
}
