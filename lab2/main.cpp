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
#include "Event.h"
#include "Scheduler.h"
#include "Des.h"

using namespace std;

// global variables
bool if_verbose; // whether verbose output
bool IF_DEBUG = true;
int quantum; // quantum for RR and PRIO only
std::ifstream ifs; // the input file
Scheduler *schdulr;
Des des;
vector<Process> processes;
sch sch_type;

//Process empty_process; // and empty process

//Event empty_event = Event(0, 0, 0, 0, empty_process, Empty); // and empty process
//Event empty_event = Event(empty_process, Empty); // and empty process


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
        rand_name = argv[4];
        if_verbose = false;
    } else {
        sch_name = argv[3];
        input_name = argv[4];
        rand_name = argv[5];
        if_verbose = true;
    }
    // decide the type of scheduler
    // get the quantum if RR or PRIO
    unordered_map<char, sch> type_map{
            {'F', FCFS},
            {'L', LCFS},
            {'S', SJF},
            {'R', RR},
            {'P', PRIO}
    };
    // read the quantum
    sch_type = type_map[sch_name[0]];
    if (sch_type == RR | sch_type == PRIO)
        quantum = stoi(sch_name.substr(1, string::npos));
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
    int i = 0;
    while (getline(ifs, l)) {
        string token;
        istringstream iss(l);
        int a[4];
        int j = 0;
        while (iss >> token)
            a[j++] = stoi(token);
        if (j != 4) {
            cout << "Error reading processes.\n";
            exit(1);
        }
        // assume all the processes are increasing arrival time order
        processes.push_back(Process(i, a[0], a[1], a[2], a[3], my_random(4)));
        i++;
    }
    // init the scheduler
    switch (sch_type) {
//        case FCFS:
//            schdulr = Scheduler_fcfs(processes);
        case RR:
            schdulr = new Scheduler_rr(processes, quantum);
    }
    // init the DES layer
    for (Process &s : processes)
//        des.insert(Event(s->time, s->arrival, s, Created, Ready));
        des.insert(Event(s.time, s.arrival, &s, Created, Ready));
    if (IF_DEBUG)
        for (Process &s : processes)
            s.print();
}

int main(int argc, char *argv[]) {
    init(argc, argv);
    // the whole of simulation
    // TODO: I first assume there is no spontaneous event
    while (schdulr->running()) {

        // the next event
        Event e1 = des.pop();
        switch (e1.to) {
            case Blocked:
                Process *cp = e1.process;
                des.insert(Event(cp->time, e1.trigger + e1.burst, cp, Blocked, Ready));
        }
        Event e2 = schdulr->next(e1);
        if (e2.to != Empty)
            des.insert(e2);
    }
    delete schdulr;
}

