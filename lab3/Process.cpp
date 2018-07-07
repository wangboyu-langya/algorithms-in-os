//
// Created by temporary on 2018/7/6.
//

#include "Process.h"

bool Process::legal(int i) {
    if (page_table.find(i) == page_table.end())
        return true;
    else
        return false;
}

Process::Process(int p) {
    pid = p;
}

void Process::insert(unsigned int lb, unsigned int ub, unsigned int protect, unsigned int file_map) {
	page_table.clear();
	page_table.reserve(ub - lb + 1);
    for (unsigned i = lb; i < ub; ++i) {
//        Pte p(0, protect, 0, 0, 0, 0, file_map);
//        page_table[i] = p;
//        page_table[i] = Pte(0, protect, 0, 0, 0, 0, file_map);
//        page_table.insert({i, Pte(0, protect, 0, 0, 0, 0, file_map)});
        Pte tmp( 0, protect, 0, 0, 0, 0, file_map );
		page_table.emplace(i, tmp);
    }
}
