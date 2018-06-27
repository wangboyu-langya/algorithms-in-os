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
            else if (aft != events.end() && e.trigger < (*aft).trigger) {
                events.insert(aft, e);
                break;
            }
                // case 3: move the iterator
            else {
                // TODO: case 4 should be resolved in scheduler instead of Des layer
//                        // case 4: two events at the same time
//                        if (e.trigger != (*aft).trigger)
//                            // whether they are attached to the same process
//                            if (e.process == (*bef).process)
//                                if (e.type == Done)
//                                    events.insert(bef, e);
//                                else if (e.type == )
                // ensure aft doesn't go beyond
                if (aft != events.end()) aft++;
                // e is the latest, attached to the end
                if (bef != events.end()) bef++;
                else
                    events.push_back(e);
            }

    }
}

Event Des::pop() {
    Event e = events.front();
    events.pop_front();
    return e;
}
