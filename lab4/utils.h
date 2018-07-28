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

// define an alias Time
using Time = int;


struct Request {
    Time arrival;
    int track;
    int id;
    Time submit;
    Time start;
    Time finish;
    Request(int rid, Time a, int t);
};


#endif //LAB02_UTILS_H
