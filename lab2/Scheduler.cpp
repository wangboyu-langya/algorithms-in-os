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
    if (cp->cpu_burst == 0) {
        int cb = my_random(cp->cb_seed);
        if (cb > cp->remain_cpu)
            cb = cp->cb_seed;
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
    // the general philosophy is to print the state at from
    switch (e.to) {
        case Ready:
            // a new process is ready
            switch (e.from) {
                case Created:
                    cout << e.trigger << ' ' << cp->pid << ' ' << e.trigger - e.last_trigger << ':'
                         << "CREATED -> READY"
                         << endl;
                    break;
                case Blocked:
                    cout << e.trigger << ' ' << cp->pid << ' ' << e.trigger - e.last_trigger << ':'
                         << "Blocked -> READY"
                         << endl;
                    break;
                case Running:
                    cout << e.trigger << ' ' << e.process->pid << ' ' << e.trigger - e.last_trigger << ':'
                         << "Running -> READY  "
                         << ' ' << "cb=" << cp->cpu_burst << " rem=" << cp->remain_cpu << " prio=" << cp->prio
                         << endl;
                    break;
            }
            // push it to ready state
            ready.push_back(cp);
            // refresh timing
            cp->time = e.trigger;
            cp->status = Ready;
            if (run.empty()) {
                // if no process is running, pick a process to run
                // in Round Robin, the first in ready list is chosen
                return get(e.trigger);
            } else
                return Event(cp, Empty);
        case Running:
            // a running process has finished
            // current runned process, update it
            // do the arithmetic
            cout << e.trigger << ' ' << e.process->pid << ' ' << e.trigger - e.last_trigger << ':'
                 << "READY -> Running"
                 << ' ' << "cb=" << cp->cpu_burst << " rem=" << cp->remain_cpu << " prio=" << cp->prio
                 << endl;
            cp->remain_cpu -= e.burst;
            cp->cpu_burst -= e.burst;
            // refresh timing
            cp->time = e.trigger;
            cp->status = Running;
            // check the event matches
            if (run.empty()) { cerr << "Nothing is running!" << endl; }
//            else if (cp != run.front()) { cerr << "Runing process doens't match!" << endl; }
            else { run.pop_front(); }
            // check whether the process has ended
            if (cp->remain_cpu == 0) {
                ps.remove(cp);
                finished.push_back(cp);
                return Event(cp->time, e.trigger + e.burst, cp, Running, Done);
            } else {
                // allocate I/O if cpu burst is used up
                if (cp->cpu_burst == 0) {
                    int io = my_random(cp->io_seed);
                    return Event(cp->time, e.trigger + e.burst, io, cp, Running, Blocked);
                } else
                    return Event(cp->time, e.trigger + e.burst, cp, Running, Ready);
            }
        case Blocked:
            // insert them to ready
            cout << e.trigger << ' ' << e.process->pid << ' ' << e.trigger - e.last_trigger << ':'
                 << "Running -> Blocked"
                 << ' ' << "cb=" << e.burst << "rem=" << cp->remain_cpu << "prio=" << cp->prio
                 << endl;
            // refresh timing
            cp->time = e.trigger;
            cp->status = Blocked;
//            des.insert(Event(cp->time, e.trigger + e.burst, cp, Blocked, Ready));
            // get a new process to run
            return get(e.trigger + e.burst);
        case Done:
            cout << e.trigger << ' ' << e.process->pid << ' ' << e.trigger - e.last_trigger << ':'
                 << "Done"
                 << endl;
            // refresh timing
            cp->time = e.trigger;
            cp->status = Done;
            if (ready.empty())
                return Event(cp, Empty);
            else
                return get(e.trigger + e.burst);
    }
}
