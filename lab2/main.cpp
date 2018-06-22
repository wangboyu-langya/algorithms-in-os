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
#include <list>

using namespace std;

bool if_verbose; // whether verbose output
bool IF_DEBUG = true;
int quantum; // quantum for RR and PRIO only
std::ifstream ifs, rfs; // the input file
vector<int> rand_v; // random vector
vector<int>::iterator rand_it;
// scheduler type
enum sch {
    FCFS, LCFS, SJF, RR, PRIO
};
sch sch_type;

enum state {
    Created, Ready, Running, Blocked
};

enum event_type {
    C2Re, Re2Ru, Ru2B, B2Re, Ru2Re, Done
};

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

    Process(int p, int at, int tc, int cb, int ib, int pr) {
        pid = p;
        arrival = at;
        total_cpu = tc;
        cpu_burst = cb;
        io_burst = ib;
        prio = pr;
    }

    void print() {
        cout << pid << ' ' << arrival << ' ' << total_cpu << ' ' << cpu_burst << ' ' << io_burst << " prio=" << prio
             << endl;
    }
};

class Event {
public:
    int trigger; // time the event should be triggered
    Process &process; // process the event is attached to
    event_type &type; // event type
    Event(int t, Process &p, event_type &tp) {
        trigger = t;
        process = p;
        type = tp;
    }
};

class Des {
public:
    // with vector insert takes O(n), search takes O(lgn)
    // with list insert takes O(1), search takes O(n)
    // so basically they are the same
    // here I choose list sice this is easier to implement
    list<&Event> events;

    // insert an event in ascending order
    void insert(Event &e) {
        if (events.empty())
            events.push_back(e);
        else {
            // the event before
            list<&Event>::iterator bef = events.begin();
            // the event after
            list<&Event>::iterator aft = events.begin();
            aft++;
            // case 1: insert the event before the previous one if earlier
            while (bef != events.end())
                if (e.trigger < (*bef).trigger) {
                    events.insert(bef, e);
                    break;
                }
                    // case 2: insert the event between the previous and the latter one
                else if (aft != events.end() && e.trigger < (*aft).trigger) {
                    events.insert(aft, e);
                    break;
                }
                    // case 3: move the iterator
                else {
                    // TODO: case 4 should be resolved in scheduler instead of Des layer
//                        // case 4: two events at the same time
//                        if (e.trigger != (*aft).trigger)
//                            // whether they are attached to the same process
//                            if (e.process == (*bef).process)
//                                if (e.type == Done)
//                                    events.insert(bef, e);
//                                else if (e.type == )
                    // ensure aft doesn't go beyond
                    if (aft != events.end()) aft++;
                    // e is the latest, attached to the end
                    if (bef != events.end()) bef++;
                    else
                        events.push_back(e);
                }

        }
    }

    // return the first element of the list and delete it
    // TODO: need to check the reference is valid
    Event& pop() {
        Event &e = events.front();
        events.pop_front();
        return e;
    }
};

vector<Process> processes;

void load_rand(string &rand_name) {
    // read random number into an array
    rfs.open(rand_name);
    if (!rfs) {
        cout << "Cannot open random file.\n";
        exit(1);
    }
    // temporary variables to read random number
    string rand_s;
    rfs >> rand_s;
    int len = stoi(rand_s);
    while (rfs >> rand_s)
        rand_v.push_back(stoi(rand_s));
    // make sure this process is right
    if (rand_v.size() != len) {
        cerr << "Error reading random file, the number doesn't match.\n";
        exit(1);
    }
    rand_it = rand_v.begin();
}

int my_random(int burst) {
    // use vector iterator, if reaches the end, reload the iterator
    if (rand_it == rand_v.end())
        rand_it = rand_v.begin();
    return *rand_it++ % burst + 1;
}

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
    if (IF_DEBUG)
        for (Process &s : processes)
            s.print();
}

class Scheduler {
public:
    vector<Process&> processes;
    virtual Event &next();
};

class Schdulr_fcfs : public Scheduler{
}

int main(int argc, char *argv[]) {
    init(argc, argv);
}
