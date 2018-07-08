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
        frames_free.push_back(&frames.back());
    }
}

Frame *Pager_fifo::get() {
    Frame *f;
    if (!frames_free.empty()) {
        f = frames_free.front();
        frames_free.pop_front();
        frames_occupied.push_back(f);
    } else {
        f = frames_occupied.front();
        frames_occupied.pop_front();
        frames_occupied.push_back(f);
    }
    return f;
}

Frame *Pager_sec::get() {
    Frame *f;
    if (!frames_free.empty()) {
        f = frames_free.front();
        frames_free.pop_front();
        frames_occupied.push_back(f);
    } else {
        for(auto fr:frames_occupied) {
            if ()
        }
        f = frames_occupied.front();
        frames_occupied.pop_front();
        frames_occupied.push_back(f);
    }
    return f;
}
