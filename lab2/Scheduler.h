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
    list<reference_wrapper<Process>> ps; // list of all the processes
//    list<Process &> run; // running processes
    list<reference_wrapper<Process>> run; // running processes
//    list<Process &> ready; // ready processes
    list<reference_wrapper<Process>> ready; // running processes
//    list<Process &> blocked; // ready processes
//    list<Process &> preempted; // ready processes
//    list<Process &> finished; // ready processes
    list<reference_wrapper<Process>> finished; // running processes

    Scheduler() = default;

    Scheduler(vector <Process> p);

    // next event based on the finished event
    virtual Event next(Event &e);

    // get running process
    virtual Event get(int t);

    // whether the scheduler finishes
    bool running();
};


class Scheduler_rr : public Scheduler {
public:
    int quantum;
    explicit Scheduler_rr(vector<Process> &p, int q);
    // get another process to run
    Event get(int t) override;
    Event next(Event &e) override;
};

#endif //LAB02_SCHEDULER_H
