//
// Created by temporary on 2018/6/26.
//

#include "Scheduler.h"

bool Scheduler::idle() {
    return processing.empty();
}

void Scheduler::insert(Request *r) {
    ready->push_back(r);
}

bool Scheduler::is_todo() {
    return !ready_list.empty();
}

void Scheduler_fifo::select(Time &t) {
    if (!ready->empty()) {
        Request *r = ready->front();
        r->start = t;
        r->finish = abs(r->track - head) + t;
        processing.push_back(r);
        ready->pop_front();
    }
}

void Scheduler_sstf::select(Time &t) {
    if (!ready->empty()) {
        auto min = ready->begin();
        int distance = abs(ready->front()->track - head);
        // find the closet track
        for (auto it = ready->begin(); it != ready->end(); it++) {
            int d = abs((*it)->track - head);
            if (d < distance) {
                min = it;
                distance = d;
            }
        }
        // get it and then erase
        Request *r = *min;
        ready->erase(min);
        r->start = t;
        r->finish = distance + t;
        processing.push_back(r);
    }
}

void Scheduler_look::select(Time &t) {
    if (!ready->empty()) {
        auto min = ready->begin();
        int distance = -1; // make sure the distance it positive
        // find the closet track
        auto it = ready->begin();
        while (true) {
            int d = (*it)->track - head;
            switch (up) {
                case true:
                    if (d >= 0) {
                        if (distance == -1) {
                            distance = d;
                            min = it;
                        } else if (d < distance) {
                            distance = d;
                            min = it;
                        }
                    }
                    break;
                case false:
                    if (d <= 0) {
                        if (distance == -1) {
                            distance = -d;
                            min = it;
                        } else if (-d < distance) {
                            distance = -d;
                            min = it;
                        }
                    }
                    break;
            }
            it++;
            // if the request is not found in the same direction, then reverse the direction and search again
            if (it == ready->end()) {
                if (distance == -1) {
                    it = ready->begin();
                    up = !up;
                } else
                    break;
            }
        }
        // get it and then erase
        Request *r = *min;
        ready->erase(min);
        r->start = t;
        r->finish = distance + t;
        processing.push_back(r);
    }
}

void Scheduler_clook::select(Time &t) {
    if (!ready->empty()) {
        auto min = ready->begin();
        int distance = -1; // make sure the distance it positive
        auto boundary = min; // boundary condition
        int b = (*min)->track; // boundary track
        // find the closet track
        auto it = ready->begin();
        while (true) {
            int d = (*it)->track - head;
            // in the clook algo, the direction is never changed
            switch (up) {
                case true:
                    if (d >= 0) {
                        if (distance == -1) {
                            distance = d;
                            min = it;
                        } else if (d < distance) {
                            distance = d;
                            min = it;
                        }
                    } else {
                        if ((*it)->track < b) {
                            b = (*it)->track;
                            boundary = it;
                        }
                    }
                    break;
            }
            it++;
            // if the request is not found in the same direction, then reverse the direction and search again
            if (it == ready->end()) {
                if (distance == -1) {
                    min = boundary;
                    distance = abs((*boundary)->track - head);
                }
                break;
            }
        }
        // get it and then erase
        Request *r = *min;
        ready->erase(min);
        r->start = t;
        r->finish = distance + t;
        processing.push_back(r);
    }
}

void Scheduler_flook::insert(Request *r) {
    switch (in_process) {
        case -1:
            ready_list.push_back(r);
            in_process = 0;
            break;
        case 0:
            ready_list_b.push_back(r);
            break;
        case 1:
            ready_list.push_back(r);
            break;
    }
}


bool Scheduler_flook::is_todo() {
    return !(ready_list.empty() && ready_list_b.empty());
}

void Scheduler_flook::select(Time &t) {
    if (is_todo()) {
        if (ready->empty()) {
            if (in_process == 0) {
                in_process = 1;
                ready = &ready_list_b;
            } else {
                in_process = 0;
                ready = &ready_list;
            }
        }
        auto min = ready->begin();
        int distance = -1; // make sure the distance it positive
        // find the closet track
        auto it = ready->begin();
        while (true) {
            int d = (*it)->track - head;
            switch (up) {
                case true:
                    if (d >= 0) {
                        if (distance == -1) {
                            distance = d;
                            min = it;
                        } else if (d < distance) {
                            distance = d;
                            min = it;
                        }
                    }
                    break;
                case false:
                    if (d <= 0) {
                        if (distance == -1) {
                            distance = -d;
                            min = it;
                        } else if (-d < distance) {
                            distance = -d;
                            min = it;
                        }
                    }
                    break;
            }
            it++;
            // if the request is not found in the same direction, then reverse the direction and search again
            if (it == ready->end()) {
                if (distance == -1) {
                    it = ready->begin();
                    up = !up;
                } else
                    break;
            }
        }
        // get it and then erase
        Request *r = *min;
        ready->erase(min);
        r->start = t;
        r->finish = distance + t;
        processing.push_back(r);
    }
}

