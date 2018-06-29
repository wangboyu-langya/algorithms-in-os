//
// Created by temporary on 2018/6/26.
//

#ifndef LAB02_SCHEDULER_H
#define LAB02_SCHEDULER_H


#include "Process.h"
#include "Event.h"
#include "utils.h"
#include <iostream>
#include <list>
#include <vector>

using namespace std;

class Scheduler {
public:
    list<Process *> ps; // list of all the processes
    list<Process *> run; // running processes
    list<Process *> ready_list; // running processes
    list<Process *>* ready = &ready_list; // running processes
    list<Process *> expire_list; // running processes
    list<Process *>* expire = &expire_list; // running processes
    list<Process *> blocked; // running processes
    list<Process *> finished; // running processes
    int io_waste = 0;
    int last_io_check = 0;
    io_state last_io_status = Vacant;

    Scheduler() = default;

    Scheduler(vector<Process> &p);

    // next event based on the finished event
//    virtual Event next(Event &e)= 0;
    // get running process
    virtual Event get(int t)= 0;

    // whether the scheduler finishes
    bool running();

    void report(vector<Process> &processes, sch type, int quantum, int t);
};

class Scheduler_prio : public Scheduler {
public:
    int quantum;

    explicit Scheduler_prio(vector<Process> &p, int q);

    // get another process to run
    Event get(int t) override;
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
