//
// Created by temporary on 2018/7/6.
//

#include "Process.h"

bool Process::legal(int i) {
    if (page_table.find(i) != page_table.end())
        return true;
    else
        return false;
}

Process::Process(int p) {
    pid = p;
}

void Process::insert(int lb, int ub, int protect, int file_map) {
    for (int i = lb; i < ub; ++i)
        page_table[i] = Pte(0, protect, 0, 0, 0, 0, file_map);
}
