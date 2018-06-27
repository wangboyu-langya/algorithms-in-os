//
// Created by temporary on 2018/6/26.
//

#include "Process.h"
#include <iostream>

Process::Process(int p, int at, int tc, int c, int i, int pr) {
    pid = p;
    arrival = at;
    time = at;
    total_cpu = tc;
    io_seed = i;
    remain_cpu = tc;
    cb_seed = c;
    prio = pr;
}

void Process::print() {
    std::cout << pid << ' ' << arrival << ' ' << total_cpu << ' ' << remain_cpu << ' ' << "prio=" << prio
         << std::endl;
}
