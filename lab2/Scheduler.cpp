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

Scheduler_rr::Scheduler_rr(vector<Process> &p, int q) : Scheduler(p), quantum(q) {}

Event Scheduler_rr::get(int t) {
    Process *cp = ready.front();
    ready.pop_front();
    run.push_back(cp);
    // if the cpu burst has been used up, allocate a new one

//    if (t == 510)
//        cout << "stop" << endl;
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
    Process *cp = ready.front();
    ready.pop_front();
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
    Process *cp = ready.back();
    ready.pop_back();
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
    if (t == 67)
        cout << "stop" << endl;
    Process *cp = ready.front();
    for (auto p : ready) {
        if(p->remain_cpu < cp->remain_cpu)
            cp = p;
    }
    ready.remove(cp);
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



