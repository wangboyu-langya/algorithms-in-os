//
// Created by temporary on 2018/7/6.
//

#ifndef LAB03_PROCESS_H
#define LAB03_PROCESS_H

#include "utils.h"
#include <unordered_map>

using namespace std;
class Process {
public:
    Process(int p);

    std::unordered_map<int, Pte> page_table;
    int pid;

    // init pte for the given address
    void insert(int lb, int ub, int protect, int file_map);

    // if a given virtual page is legal
    bool legal(int i);
};


#endif //LAB03_PROCESS_H
