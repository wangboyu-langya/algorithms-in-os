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

Event Scheduler_rr::next(Event &e) {
    Process *cp = e.process;
    int current_time = e.trigger;
    switch (e.to) {
        case Running:
            // check whether the process has ended
            if (cp->remain_cpu == 0) {
                ps.remove(cp);
                finished.push_back(cp);
                return Event(cp->time, current_time + e.burst, cp, Running, Done);
            } else {
                // allocate I/O if cpu burst is used up
                if (cp->cpu_burst == 0) {
                    int io = my_random(cp->io_seed);
                    return Event(cp->time, current_time + e.burst, io, cp, Running, Blocked);
                } else
                    return Event(cp->time, current_time + e.burst, cp, Running, Ready);
            }
    }
}
