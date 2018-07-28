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
#include "Scheduler.h"

using namespace std;

// global variables
list<Request> requests;
Scheduler *scheduler;

bool if_debug = false;

// init the arguments and input files
void init(int argc, char *argv[]) {
    if (argc != 3 && argc != 4) {
        cout << "Please use: lab04.exe -s[ijscf] inputfile" << endl;
        exit(0);
    }
    if (argc == 4)
        if_debug = true;
    string input_name;
    // whether there is verbose output
    input_name = argv[2];
    for (int i = 1; i < argc; ++i) {
        string tmp = argv[i];
        if (tmp[0] == '-')
            switch (tmp[2]) {
                case 'i':
                    scheduler = new Scheduler_fifo;
                    break;
                case 'j':
                    scheduler = new Scheduler_sstf;
                    break;
                case 's':
                    scheduler = new Scheduler_look;
                    break;
                case 'c':
                    scheduler = new Scheduler_clook;
                    break;
                case 'f':
                    scheduler = new Scheduler_flook;
                    break;
            }
    }
    std::ifstream ifs; // the input file
    ifs.open(input_name);
    if (!ifs) {
        cout << "Cannot open input file.\n";
        exit(1);
    }
    // temporary variables to read processes
    string l;
    int arrival, track;
    int counter = 0;
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
            arrival = stoi(token);
            iss >> token;
            track = stoi(token);
            if (if_debug)
                cout << arrival << " " << track << endl;
            requests.push_back(Request(counter++, arrival, track));
        }
    }
}

int main(int argc, char *argv[]) {
    init(argc, argv);
    // init the time
    Time time = 0;
    std::list<Request>::iterator it = requests.begin();
    // as long as the task is not over
    while (!(it == requests.end() && scheduler->idle())) {
        if (time == 109)
            cout << "Stop" << endl;
        // add a new request to the ready state
        if (it != requests.end()) {
            if (it->arrival == time) {
                scheduler->insert(&(*it));
                it->submit = time;
                it++;
            }
        }
        // notify the scheduler the time
        // all the dirty stuff is left to the scheduler
        if (!scheduler->idle()) {
            // pop a finished request
            if (scheduler->processing.front()->finish == time) {
                Request *req = scheduler->processing.front();
                printf("%5d: %5d %5d %5d %5d\n", scheduler->io_times++, req->track, req->arrival, req->start, req->finish);
                // reset the head
                scheduler->processing.pop_front();
                scheduler->head_moves += abs(req->track - scheduler->head);
                scheduler->head = req->track;
            }
        }
        if (scheduler->idle())
            // select a new one, this is where the difference is
            scheduler->select(time);
        // the time increment seems complicated just in case two events happen at the same time
        if (!scheduler->idle()) {
            if (scheduler->processing.front()->finish != time)
                time++;
        } else
            time++;
    }
    for (auto it = requests.begin(); it != requests.end(); it++) {
        if (if_debug)
            printf("%5d: %5d %5d %5d\n", it->id, it->arrival, it->start, it->finish);
    }
    // do the statistics
    float avg_turnaround = 0;
    float avg_waittime = 0;
    int max_waittime = 0;
    for (it = requests.begin(); it != requests.end(); it++) {
        avg_turnaround += it->finish - it->submit;
        int wait = it->start - it->submit;
        avg_waittime += wait;
        if (wait > max_waittime)
            max_waittime = wait;
    }
    avg_turnaround /= scheduler->io_times;
    avg_waittime /= scheduler->io_times;
    printf("SUM: %d %d %.2lf %.2lf %d\n",
           --time, scheduler->head_moves, avg_turnaround, avg_waittime, max_waittime);
    delete scheduler;
}

