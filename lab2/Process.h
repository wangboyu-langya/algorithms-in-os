//
// Created by temporary on 2018/6/26.
//

#ifndef LAB02_PROCESS_H
#define LAB02_PROCESS_H


#include "utils.h"

class Process {
public:
    int pid;
    int finish; // finish time
    int turn_around; // turnaround time
    int prio; // priority
    int arrival; // arrival time
    int total_cpu;
    int remain_cpu; // remaining cpu burst
    int io_seed;
    int cb_seed;
    int io_burst;
    int cpu_burst;
    int time;
    state status;

    Process() = default;

    Process(int p, int at, int tc, int c, int i, int pr = 0);

    void print();
};


#endif //LAB02_PROCESS_H
