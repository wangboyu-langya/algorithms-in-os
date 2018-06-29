//
// Created by temporary on 2018/6/26.
//

#include "Event.h"
#include "Process.h"

Event::Event(int t1, int t2, Process *p, state f, state t) : last_trigger(t1), trigger(t2), process(p), from(f),
                                                             to(t) {}

Event::Event(int t1, int t2, int b, Process *p, state f, state t) : last_trigger(t1), trigger(t2), burst(b), process(p),
                                                                    from(f), to(t) {}
