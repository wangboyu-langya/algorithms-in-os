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
bool verbose; // whether verbose output
bool IF_DEBUG = false;
bool call_scheduler = false;
int quantum = 0; // quantum for RR and PRIO only
std::ifstream ifs; // the input file
Scheduler *schdulr;
Des des;
vector<Process> processes;
sch sch_type;

// init the arguments and input files
void init(int argc, char *argv[]) {
    if (argc < 4 | argc > 5) {
        cout << "Please use: lab02.exe [-v] -s[FLS|R<num>|P<num>] inputfile randfile" << endl;
        exit(0);
    }
    string input_name, rand_name, sch_name;
    // whether there is verbose output
    if (argc == 4) {
        sch_name = argv[1];
        input_name = argv[2];
        rand_name = argv[3];
        verbose = false;
    } else {
        sch_name = argv[2];
        input_name = argv[3];
        rand_name = argv[4];
        verbose = true;
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
    sch_type = type_map[sch_name[2]];
    if (sch_type == RR | sch_type == PRIO)
        quantum = stoi(sch_name.substr(3, string::npos));
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
        case PRIO:
            schdulr = new Scheduler_prio(processes, quantum);
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
    int current_time;
    while (!des.events.empty()) {
        // the next event
        Event e = des.pop();
        current_time = e.trigger;
        while (true) {
            Process *cp = e.process;
            int last_time = e.last_trigger;
            // refresh timing
            cp->time = current_time;
            // statistics
            switch (e.from) {
                case Blocked:
                    cp->io_time += current_time - last_time;
                    schdulr->blocked.remove(cp);
                    break;
                case Ready:
                    cp->cpu_wait += current_time - last_time;
                    break;
            }
            // the general philosophy is to print the state at from
            switch (e.to) {
                case Ready:
                    // a new process is ready
                    switch (e.from) {
                        case Created:
                            if (verbose)
                                cout << current_time << ' ' << cp->pid << ' ' << current_time - last_time << ": "
                                     << "CREATED -> READY"
                                     << endl;
                            // push it to ready state
                            schdulr->ready->push_back(cp);
                            break;
                        case Blocked:
                            if (verbose)
                                cout << current_time << ' ' << cp->pid << ' ' << current_time - last_time << ": "
                                     << "BLOCK -> READY"
                                     << endl;
                            // push it to ready state
                            schdulr->ready->push_back(cp);
                            break;
                        case Running:
                            if (verbose)
                                cout << current_time << ' ' << e.process->pid << ' ' << current_time - last_time << ": "
                                     << "RUNNG -> READY"
                                     << ' ' << "cb=" << cp->cpu_burst << " rem=" << cp->remain_cpu << " prio="
                                     << cp->prio
                                     << endl;
                            // lower the priority whenever a cpu burst has finished
                            // this is reserved to be compatible with the given output
                            if (sch_type == PRIO) {
                                cp->prio--;
                                if (cp->prio == -1) {
                                    cp->prio = cp->prio_static - 1;
                                    // push it to expire state
                                    schdulr->expire->push_back(cp);
                                } else
                                    // push it to ready state
                                    schdulr->ready->push_back(cp);
                            } else
                                // push it to ready state
                                schdulr->ready->push_back(cp);
                            // delete it from the running
                            schdulr->run.pop_front();
                            break;
                    }
                    break;
                case Running:
                    if (verbose)
                        cout << current_time << ' ' << e.process->pid << ' ' << current_time - last_time << ": "
                             << "READY -> RUNNG"
                             << ' ' << "cb=" << cp->cpu_burst << " rem=" << cp->remain_cpu << " prio=" << cp->prio
                             << endl;
                    // do the arithmetic
                    cp->remain_cpu -= e.burst;
                    cp->cpu_burst -= e.burst;
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
                    if (verbose)
                        cout << current_time << ' ' << e.process->pid << ' ' << current_time - last_time << ": "
                             << "RUNNG -> BLOCK"
                             << ' ' << "ib=" << e.burst << " rem=" << cp->remain_cpu << endl;
                    // lower the priority whenever a cpu burst has finished
                    if (sch_type == PRIO) cp->prio = cp->prio_static - 1;
                    schdulr->blocked.push_back(cp);
                    // delete it from the running
                    schdulr->run.pop_front();
                    des.insert(Event(cp->time, current_time + e.burst, cp, Blocked, Ready));
                    break;
                case Done:
                    if (verbose)
                        cout << current_time << ' ' << e.process->pid << ' ' << current_time - last_time << ": "
                             << "Done"
                             << endl;
                    cp->finish = current_time;
                    cp->turn_around = cp->finish - cp->arrival;
                    // delete it from the running
                    schdulr->run.pop_front();
            }
            // break if there is no next event
            if (des.next(current_time))
                e = des.pop();
            else {
                // do some statistics
                if (schdulr->last_io_status == Vacant)
                    schdulr->io_waste += (current_time - schdulr->last_io_check);
                schdulr->last_io_check = current_time;
                if (schdulr->blocked.empty())
                    schdulr->last_io_status = Vacant;
                else schdulr->last_io_status = Busy;
                break;
            }
        }
        // if the cpu is not occupied, assign it
        if (schdulr->run.empty() && (!schdulr->ready->empty() | !schdulr->expire->empty())) {
            Event e2 = schdulr->get(current_time);
            des.insert(e2);
        }
    }
    schdulr->report(processes, sch_type, quantum, current_time);
    // remember to deleter the scheduler in the heap
    delete schdulr;
}

