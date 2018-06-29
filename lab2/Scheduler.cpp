//
// Created by temporary on 2018/6/26.
//

#include "Scheduler.h"

Scheduler::Scheduler(vector<Process> &p) {
    for (auto &r : p)
        ps.push_back(&r);
}

bool Scheduler::running() {
    if (ps.empty())
        return false;
    else
        return true;
}

void Scheduler::report(vector<Process> &processes, sch type, int quantum, int t) {
    // print the first line
    switch (type) {
        case FCFS:
            cout << "FCFS" << endl;
            break;
        case LCFS:
            cout << "LCFS" << endl;
            break;
        case SJF:
            cout << "SJF" << endl;
            break;
        case RR:
            cout << "RR " << quantum << endl;
            break;
        case PRIO:
            cout << "PRIO " << quantum << endl;
            break;
    }
    // per process info
    double cpu_util = 0;
    double io_util = t - io_waste;
    double turn_around = 0;
    double cpu_wait = 0;
    double num_p = finished.size();
    double through_put = num_p / t * 100;
    // statistics
    for (auto &p : processes) {
        p.report();
        cpu_util += p.total_cpu;
        turn_around += p.turn_around;
        cpu_wait += p.cpu_wait;
    }
    cpu_util /= t;
    cpu_util *= 100;
    io_util /= t;
    io_util *= 100;
    turn_around /= num_p;
    cpu_wait /= num_p;
    printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n",
           t,
           cpu_util,
           io_util,
           turn_around,
           cpu_wait,
           through_put);
}

Scheduler_prio::Scheduler_prio(vector<Process> &p, int q) : Scheduler(p), quantum(q) {}

Event Scheduler_prio::get(int t) {
    // exchange ready and expire if necessary
    if (ready->empty()){
        list<Process*>* tmp = ready;
        ready = expire;
        expire = tmp;
    }
    // find the one with the highest priority
    Process *cp = ready->front();
    if (cp->prio != 3) {
        for (auto r : *ready) {
            if (r->prio > cp->prio)
                cp = r;
        }
    }
    ready->remove(cp);
    run.push_back(cp);
    // if the cpu burst has been used up, allocate a new one
    if (cp->cpu_burst == 0) {
        int cb = my_random(cp->cb_seed);
        if (cb > cp->remain_cpu)
            cb = cp->remain_cpu;
        cp->cpu_burst = cb;
    }
    // if the burst is larger than the quantum
    if (cp->cpu_burst > quantum) {
        return Event(cp->time, t, quantum, cp, Ready, Running);
    } else {
        return Event(cp->time, t, cp->cpu_burst, cp, Ready, Running);
    }
}


Scheduler_rr::Scheduler_rr(vector<Process> &p, int q) : Scheduler(p), quantum(q) {}


Event Scheduler_rr::get(int t) {
    Process *cp = ready->front();
    ready->pop_front();
    run.push_back(cp);
    // if the cpu burst has been used up, allocate a new one
    if (cp->cpu_burst == 0) {
        int cb = my_random(cp->cb_seed);
        if (cb > cp->remain_cpu)
            cb = cp->remain_cpu;
        cp->cpu_burst = cb;
    }
    // if the burst is larger than the quantum
    if (cp->cpu_burst > quantum) {
        return Event(cp->time, t, quantum, cp, Ready, Running);
    } else {
        return Event(cp->time, t, cp->cpu_burst, cp, Ready, Running);
    }
}

Event Scheduler_fcfs::get(int t) {
    // get the first one in the front
    Process *cp = ready->front();
    ready->pop_front();
    run.push_back(cp);
    // allocate a new one
    if (cp->cpu_burst == 0) {
        int cb = my_random(cp->cb_seed);
        if (cb > cp->remain_cpu)
            cb = cp->remain_cpu;
        cp->cpu_burst = cb;
        return Event(cp->time, t, cp->cpu_burst, cp, Ready, Running);
    }
}

Event Scheduler_lcfs::get(int t) {
    // get the newest one, which is on the back
    Process *cp = ready->back();
    ready->pop_back();
    run.push_back(cp);
    // allocate a new one
    if (cp->cpu_burst == 0) {
        int cb = my_random(cp->cb_seed);
        if (cb > cp->remain_cpu)
            cb = cp->remain_cpu;
        cp->cpu_burst = cb;
        return Event(cp->time, t, cp->cpu_burst, cp, Ready, Running);
    }
}

Event Scheduler_sjf::get(int t) {
    Process *cp = ready->front();
    // find the shortest
    for (auto p : *ready) {
        if (p->remain_cpu < cp->remain_cpu)
            cp = p;
    }
    ready->remove(cp);
    run.push_back(cp);
    // allocate a new one
    if (cp->cpu_burst == 0) {
        int cb = my_random(cp->cb_seed);
        if (cb > cp->remain_cpu)
            cb = cp->remain_cpu;
        cp->cpu_burst = cb;
        return Event(cp->time, t, cp->cpu_burst, cp, Ready, Running);
    }
}
