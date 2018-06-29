//
// Created by temporary on 2018/6/26.
//

#ifndef LAB02_UTILS_H
#define LAB02_UTILS_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Process.h"

using namespace std;
// enum types
// scheduler type
enum sch {
    FCFS, LCFS, SJF, RR, PRIO
};
// scheduler state
enum state {
    Created, Ready, Running, Blocked, Done
};
enum io_state {
    Busy, Vacant
};

// load a random number file
void load_rand(string &rand_name);

// return a random number
int my_random(int burst);

#endif //LAB02_UTILS_H
