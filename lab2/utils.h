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
enum state {
    Created, Ready, Running, Blocked, Done, Empty
};
enum io_state {
    Busy, Vacant
};


void load_rand(string &rand_name);

int my_random(int burst);

#endif //LAB02_UTILS_H
