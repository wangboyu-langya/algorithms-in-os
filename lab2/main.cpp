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
bool call_scheduler = false;
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
        processes.push_back(Process(i, a[0], a[1], a[2], a[3], my_random(4) - 1));
        i++;
    }
    // init the scheduler
    switch (sch_type) {
        case FCFS:
            schdulr = new Scheduler_fcfs(processes);
            break;
        case LCFS:
            schdulr = new Scheduler_lcfs(processes);
            break;
        case SJF:
            schdulr = new Scheduler_sjf(processes);
            break;
        case RR:
            schdulr = new Scheduler_rr(processes, quantum);
            break;
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
    while (!des.events.empty()) {
        // the next event
        Event e = des.pop();
        int current_time = e.trigger;
        while (true) {
//            if (current_time == 510)
//                cout << "stop" << endl;
            Process *cp = e.process;
            int last_time = e.last_trigger;
            // the general philosophy is to print the state at from
            switch (e.to) {
                case Ready:
                    // a new process is ready
                    switch (e.from) {
                        case Created:
                            cout << current_time << ' ' << cp->pid << ' ' << current_time - last_time << ':'
                                 << "CREATED -> READY"
                                 << endl;
                            break;
                        case Blocked:
                            cout << current_time << ' ' << cp->pid << ' ' << current_time - last_time << ':'
                                 << "BLOCK -> READY"
                                 << endl;
                            break;
                        case Running:
                            cout << current_time << ' ' << e.process->pid << ' ' << current_time - last_time << ':'
                                 << "RUNNG -> READY"
                                 << ' ' << "cb=" << cp->cpu_burst << " rem=" << cp->remain_cpu << " prio=" << cp->prio
                                 << endl;
                            // delete it from the running
                            schdulr->run.pop_front();
                            break;
                    }
                    // push it to ready state
                    schdulr->ready.push_back(cp);
                    // refresh timing
                    cp->time = current_time;
                    cp->status = Ready;
                    break;
                case Running:
                    // a running process has finished
                    // current runned process, update it
                    // do the arithmetic
                    cout << current_time << ' ' << e.process->pid << ' ' << current_time - last_time << ':'
                         << "READY -> RUNNG"
                         << ' ' << "cb=" << cp->cpu_burst << " rem=" << cp->remain_cpu << " prio=" << cp->prio
                         << endl;
                    cp->remain_cpu -= e.burst;
                    cp->cpu_burst -= e.burst;
                    // refresh timing
                    cp->time = current_time;
                    cp->status = Running;
                    // call the scheduler to arrange next event
                    // check whether the process has ended
                    if (cp->remain_cpu == 0) {
                        schdulr->ps.remove(cp);
                        schdulr->finished.push_back(cp);
                        des.insert(Event(cp->time, current_time + e.burst, cp, Running, Done));
                    } else {
                        // allocate I/O if cpu burst is used up
                        if (cp->cpu_burst == 0) {
                            int io = my_random(cp->io_seed);
                            des.insert(Event(cp->time, current_time + e.burst, io, cp, Running, Blocked));
                        } else
                            des.insert(Event(cp->time, current_time + e.burst, cp, Running, Ready));
                    }
                    break;
                case Blocked:
                    // insert them to ready
                    cout << current_time << ' ' << e.process->pid << ' ' << current_time - last_time << ':'
                         << "RUNNG -> BLOCK"
                         << ' ' << "ib=" << e.burst << " rem=" << cp->remain_cpu << endl;
                    // refresh timing
                    cp->time = current_time;
                    cp->status = Blocked;
                    // delete it from the running
                    schdulr->run.pop_front();
                    des.insert(Event(cp->time, current_time + e.burst, cp, Blocked, Ready));
                    break;
                case Done:
                    cout << current_time << ' ' << e.process->pid << ' ' << current_time - last_time << ':'
                         << "Done"
                         << endl;
                    // refresh timing
                    cp->time = current_time;
                    cp->status = Done;
                    // delete it from the running
                    schdulr->run.pop_front();
            }
            // break if there is no next event
            if (des.next(current_time))
                e = des.pop();
            else
                break;
        }
        if (schdulr->run.empty() && !schdulr->ready.empty()) {
            Event e2 = schdulr->get(current_time);
            des.insert(e2);
        }
    }
    delete schdulr;
}

