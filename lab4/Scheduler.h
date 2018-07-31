//
// Created by temporary on 2018/6/26.
//

#ifndef LAB02_SCHEDULER_H
#define LAB02_SCHEDULER_H


#include "utils.h"
#include <iostream>
#include <list>
#include <vector>

using namespace std;

class Scheduler {
public:
    list<Request *> ready_list;
    list<Request *>* ready = &ready_list;
    list<Request *> processing;
    int head = 0; // where the head is, start at 0
    int head_moves = 0;
    int io_times = 0.0;

    Scheduler() = default;

    bool idle(); // whether the I/O is occupied
    virtual bool is_todo();
    virtual void insert(Request *r); // add a new request
    virtual void select(Time &t) = 0; // select a suitable request
};

class Scheduler_fifo : public Scheduler {
public:
    // get another process to run
    using Scheduler::Scheduler;

    void select(Time &t) override;
};

class Scheduler_sstf : public Scheduler {
public:
    // get another process to run
    using Scheduler::Scheduler;

    void select(Time &t) override;
};

class Scheduler_look : public Scheduler {
public:
    bool up = true;
    // get another process to run
    using Scheduler::Scheduler;

    void select(Time &t) override;
};

class Scheduler_clook : public Scheduler {
public:
    bool up = true;
    // get another process to run
    using Scheduler::Scheduler;

    void select(Time &t) override;
};

class Scheduler_flook : public Scheduler {
public:
    list<Request *> ready_list_b; // extra list for flook
    bool up = true;
    int in_process = -1;
    // get another process to run
    using Scheduler::Scheduler;

    void select(Time &t) override;

    void insert(Request *r) override ; // add a new request

    bool is_todo() override ;
};

#endif //LAB02_SCHEDULER_H
