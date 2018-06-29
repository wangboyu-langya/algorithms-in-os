//
// Created by temporary on 2018/6/26.
//

#ifndef LAB02_PROCESS_H
#define LAB02_PROCESS_H


#include "utils.h"

class Process {
public:
    int pid;
    int prio; // dynamic priority
    int prio_static; // static priority
    int arrival; // arrival time
    int total_cpu;
    int remain_cpu; // remaining cpu burst
    int io_seed;
    int cb_seed;
    int cpu_burst = 0;
    int io_burst = 0;
    int total_io = 0;
    int finish = 0; // finish time
    int io_time = 0;
    int cpu_wait = 0;
    int turn_around = 0; // turnaround time
    int time;

    Process() = default;

    Process(int p, int at, int tc, int c, int i, int pr = 0);

    void print();

    void report();
};


#endif //LAB02_PROCESS_H
