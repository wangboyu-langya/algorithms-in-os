//
// Created by temporary on 2018/6/26.
//

#include "Event.h"
#include "Process.h"

Event::Event(Process *p, state t) : process(p), to(t) {}

Event::Event(int c, Process *p, state t) : last_trigger(c), process(p), to(t) {}

Event::Event(int t1, int t2, Process *p, state f, state t) : last_trigger(t1), trigger(t2), process(p), from(f), to(t) {

}

Event::Event(int t1, int t2, int b, Process *p, state f, state t) : last_trigger(t1), trigger(t2), burst(b), process(p),
                                                                    from(f),
                                                                    to(t) {

}

void Event::print() {
//        switch (type) {
//            case C2Re:
//                cout << trigger << ' ' << process.pid << ' ' << trigger - creat << ':' << "CREATED -> READY" << endl;
//                break;
//            case Re2Ru:
//                cout << trigger << ' ' << process.pid << ' ' << trigger - creat << ':' << "READY -> RUNNG" <<
//                     ' ' << process.remain_cpu << endl;
//                break;
//        }
}
