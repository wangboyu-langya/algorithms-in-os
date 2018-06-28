//
// Created by temporary on 2018/6/26.
//

#ifndef LAB02_SCHEDULER_H
#define LAB02_SCHEDULER_H


#include "Process.h"
#include "Event.h"
#include <iostream>
#include <list>
#include <vector>

using namespace std;

class Scheduler {
public:
    list<Process*> ps; // list of all the processes
    list<Process*> run; // running processes
    list<Process*> ready; // running processes
    list<Process*> finished; // running processes

    Scheduler() = default;
    Scheduler(vector <Process> &p);
    // next event based on the finished event
//    virtual Event next(Event &e)= 0;
    // get running process
    virtual Event get(int t)= 0;
    // whether the scheduler finishes
    bool running();
};


class Scheduler_rr : public Scheduler {
public:
    int quantum;
    explicit Scheduler_rr(vector<Process> &p, int q);
    // get another process to run
    Event get(int t) override;
};

class Scheduler_fcfs : public Scheduler {
public:
    // get another process to run
    using Scheduler::Scheduler;
    Event get(int t) override;
};

class Scheduler_lcfs : public Scheduler {
public:
    // get another process to run
    using Scheduler::Scheduler;
    Event get(int t) override;
};

class Scheduler_sjf : public Scheduler {
public:
    // get another process to run
    using Scheduler::Scheduler;
    Event get(int t) override;
};



#endif //LAB02_SCHEDULER_H
