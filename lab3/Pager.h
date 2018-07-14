//
// Created by temporary on 2018/7/6.
//

#ifndef LAB03_PAGER_H
#define LAB03_PAGER_H


#include <list>
#include <vector>
#include <algorithm>
#include "utils.h"
#include "Process.h"
#include <unordered_map>

using namespace std;

class Pager {
public:

    Pager();
    Pager(int fn);

    int frame_number = 0;
    vector<Frame> frames;
//    unordered_map<Frame*, Pte*> frame_table;
    unordered_map<Frame *, pair<Pte *, Process *>> frame_table;
    std::unordered_map<Pte*, Age> age_table;
    vector<Age> ages;
    list<Frame *> free;
    list<Frame *> occupied;

    virtual Frame *get() = 0;

    // init age
    void give_birth(unordered_map<int, Pte> &page_table);

    // update age
    void grow();

    // print age
    void print_age();
};

class Pager_fifo : public Pager {
public:
    using Pager::Pager;

    Frame *get();
};

class Pager_sec : public Pager {
public:
    using Pager::Pager;

    Frame *get();
};

class Pager_random : public Pager {
public:
    using Pager::Pager;

    Frame *get();
};

class Pager_nru : public Pager {
public:
    int counter = 0;
    vector<vector<Frame *>> classes;

    using Pager::Pager;

    Frame *get();

    void update();
};

class Pager_clock : Pager {
public:

    // need a specific initializer for clock
    Pager_clock(int fn);

    list<Frame *>::iterator head;

    Frame *get();

};

class Pager_aging : public Pager {
public:
    using Pager::Pager;
    Frame *get();
};

#endif //LAB03_PAGER_H
