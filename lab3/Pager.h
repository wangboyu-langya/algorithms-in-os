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
    list<Frame *> frames_free;

    Pager(int fn);

    virtual Frame *get() = 0;
};

class Pager_fifo : public Pager {
    list<Frame *> frames_occupied;
    using Pager::Pager;

    Frame *get();;
};

#endif //LAB03_PAGER_H
