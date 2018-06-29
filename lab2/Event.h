//
// Created by temporary on 2018/6/26.
//

#ifndef LAB02_EVENT_H
#define LAB02_EVENT_H

#include "utils.h"
#include "Process.h"

class Event {

public:
    int last_trigger; // time the event should be triggered
    int trigger{}; // time the event should be triggered
    int burst{}; // cpu burst or I/O burst
    Process *process; // process the event is attached to
    state from{};
    state to{};

    Event(int t1, int t2, Process *p, state f, state t);

    Event(int t1, int t2, int b, Process *p, state f, state t);
};

#endif //LAB02_EVENT_H
