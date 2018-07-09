//
// Created by temporary on 2018/7/6.
//

#ifndef LAB03_PAGER_H
#define LAB03_PAGER_H


#include <list>
#include "utils.h"

using namespace std;

class Pager {
public:
    int frame_number = 0;
    vector<Frame> frames;
    list<Frame *> free;
    list<Frame *> occupied;

    Pager(int fn);

    virtual Frame *get() = 0;
};

class Pager_fifo : public Pager {
    using Pager::Pager;

    Frame *get();;
};

class Pager_sec : public Pager {
    using Pager::Pager;

    Frame *get();;
};

class Pager_random : public Pager {
    using Pager::Pager;

    Frame *get();;
};

#endif //LAB03_PAGER_H
