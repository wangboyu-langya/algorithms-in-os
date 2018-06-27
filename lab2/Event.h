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
//    int cb;
//    int ib;
    int burst{}; // cpu burst or I/O burst
    Process *process; // process the event is attached to
//    event_type type; // event type
    state from{};
    state to{};

//    Event(int t1, int t2, int c, int i, Process &p, event_type tp) : creat(t1), trigger(t2), cb(c), ib(i), process(p),
//                                                                     type(tp) {
//    }
    Event(Process *p, state t);;

    Event(int c, Process *p, state t);;

    Event(int t1, int t2, Process *p, state f, state t);

    Event(int t1, int t2, int b, Process *p, state f, state t);

    void print();
};

#endif //LAB02_EVENT_H
