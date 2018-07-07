//
// Created by temporary on 2018/7/6.
//

#ifndef LAB03_PROCESS_H
#define LAB03_PROCESS_H

#include "utils.h"
#include <unordered_map>

class Process {
public:
    Process(int p);

    std::unordered_map<unsigned, Pte> page_table;
    int pid;

    // init pte for the given address
    void insert(unsigned int lb, unsigned int ub, unsigned int protect, unsigned int file_map);

    // if a given virtual page is legal
    bool legal(int i);
};


#endif //LAB03_PROCESS_H
