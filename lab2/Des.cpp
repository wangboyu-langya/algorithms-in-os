//
// Created by temporary on 2018/6/26.
//

#include "Des.h"

using namespace std;

void Des::insert(Event e) {
    if (events.empty())
        events.push_back(e);
    else {
        // the event before
        auto bef = events.begin();
        // the event after
        auto aft = events.begin();
        aft++;
        // case 1: insert the event before the previous one if earlier
        while (bef != events.end())
            if (e.trigger < (*bef).trigger) {
                events.insert(bef, e);
                break;
            }
                // case 2: insert the event between the previous and the latter one
            else if (aft != events.end())
                if (e.trigger < (*aft).trigger) {
                    events.insert(aft, e);
                    break;
                    // case 3: move the iterator
                } else {
                    bef++;
                    aft++;
                }
                // otherwise reach the end
            else {
                events.push_back(e);
                break;
            }

    }
}

Event Des::pop() {
    Event e = events.front();
    events.pop_front();
    return e;
}

bool Des::next(int time) {
    if (events.empty())
        return false;
    if (events.front().trigger == time)
        return true;
    else return false;
}
