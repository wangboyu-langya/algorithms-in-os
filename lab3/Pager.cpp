//
// Created by temporary on 2018/7/6.
//

#include "Pager.h"

Pager::Pager(int fn) {
    frame_number = fn;
    // init the frames
    frames.reserve(frame_number);
    for (int i = 0; i < frame_number; ++i) {
        frames.push_back(Frame(i));
        free.push_back(&frames.back());
    }
}

Frame *Pager_fifo::get() {
    Frame *f;
    if (!free.empty()) {
        f = free.front();
        free.pop_front();
        occupied.push_back(f);
    } else {
        f = occupied.front();
        occupied.pop_front();
        occupied.push_back(f);
    }
    return f;
}

Frame *Pager_sec::get() {
    Frame *f;
    if (!free.empty()) {
        f = free.front();
        free.pop_front();
        occupied.push_back(f);
    } else {
        for (auto it = occupied.begin(); it != occupied.end();) {
            Pte* pte = frame_table[*it].first;
            if (pte->referenced == 0) {
                f = *it;
                occupied.push_back(*it);
                it = occupied.erase(it);
                break;
            } else {
                pte->referenced = 0;
                occupied.push_back(*it);
                it = occupied.erase(it);
            }
        }
    }
    return f;
}

Frame *Pager_random::get() {
    Frame *f;
    if (!free.empty()) {
        f = free.front();
        free.pop_front();
        occupied.push_back(f);
    } else {
        int victim = my_random(frame_number);
        f = &frames.at(victim);
    }
    return f;
}
