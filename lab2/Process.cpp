//
// Created by temporary on 2018/6/26.
//

#include "Process.h"
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

Process::Process(int p, int at, int tc, int c, int i, int pr) {
    pid = p;
    arrival = at;
    time = at;
    total_cpu = tc;
    io_seed = i;
    remain_cpu = tc;
    cb_seed = c;
    prio_static = pr;
    prio = prio_static - 1;
}

void Process::print() {
    std::cout << pid << ' ' << arrival << ' ' << total_cpu << ' ' << remain_cpu << ' ' << "prio=" << prio
              << std::endl;
}

void Process::report() {
    printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",
           pid,
           arrival, total_cpu, cb_seed, io_seed, prio_static,
           finish, // last time stamp
           turn_around,
           io_time,
           cpu_wait);
}

