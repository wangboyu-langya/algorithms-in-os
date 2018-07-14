//
// Created by temporary on 2018/7/6.
//

#include "Pager.h"

Pager::Pager(int fn) {
    frame_number = fn;
    frames.reserve(fn);
    // init the frames
    for (int i = 0; i < frame_number; ++i) {
        frames.push_back(Frame(i));
        free.push_back(&(frames[i]));
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
            Pte *pte = frame_table[*it].first;
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

Frame *Pager_nru::get() {
    Frame *f;
    // reset the referencced bit every 10 requests
    if (!free.empty()) {
        f = free.front();
        free.pop_front();
        occupied.push_back(f);
    } else {
        counter++;
        update();
        for (auto &cls:classes)
            if (!cls.empty()) {
                int victim = my_random(cls.size());
                f = cls[victim];
                break;
            }
    }
    return f;
}

void Pager_nru::update() {
    Pte *pte;
    Frame *fp;
    // reset the classes every time
    classes.clear();
    classes.resize(4);
    for (auto &f: frames) {
        fp = &f;
        // classify the valid frames
        if (frame_table.find(fp) != frame_table.end()) {
            pte = frame_table[fp].first;
            // reset the reference bit
            switch (pte->referenced) {
                case 0:
                    switch (pte->modified) {
                        case 0:
                            classes[0].push_back(fp);
                            break;
                        case 1:
                            classes[1].push_back(fp);
                            break;
                    }
                case 1:
                    switch (pte->modified) {
                        case 0:
                            classes[2].push_back(fp);
                            break;
                        case 1:
                            classes[3].push_back(fp);
                            break;
                    }
            }
            if (counter % 10 == 0)
                pte->referenced = 0;
        }
    }
}


Pager_clock::Pager_clock(int fn) : Pager(fn) {
    head = occupied.begin();
}

Frame *Pager_clock::get() {
    Frame *f;
    Pte *pte;
    if (!free.empty()) {
        f = free.front();
        free.pop_front();
        occupied.push_back(f);
    } else
        while (true) {
            if (head == occupied.end())
                head = occupied.begin();
            if (frame_table.find(*head) != frame_table.end()) {
                pte = frame_table[*head].first;
                if (pte->referenced == 0) {
                    f = *head;
                    head++;
                    break;
                } else {
                    head++;
                    pte->referenced = 0;
                }
            }
        }
    return f;
}


Frame *Pager_aging::get() {
    Frame *f;
    Pte *pte;
    // reset the referencced bit every 10 requests
    if (!free.empty()) {
        f = free.front();
        free.pop_front();
        occupied.push_back(f);
    } else {
        // init the ages for the first time
        if (ages.empty()) {
            ages.reserve(frame_number);
            for (int i = 0; i < frame_number; ++i) {
                Frame *fp = &frames[i];
                if (frame_table.find(fp) != frame_table.end()) {
                    pte = frame_table[fp].first;
                    ages.push_back(age_table[pte]);
                }
            }
            // pick the smallest age
            auto result = min_element(ages.begin(), ages.end());
            f = &frames[std::distance(ages.begin(), result)];
        } else {
            grow();
            // pick the smallest age
            auto result = min_element(ages.begin(), ages.end());
            f = &frames[std::distance(ages.begin(), result)];
        }
    }
    return f;
}

void Pager::grow() {
    for (auto it = age_table.begin(); it != age_table.end(); ++it) {
//        if (it->first->process == 2 && it->first->virtual_page == 0)
//            cout << "stop" << endl;
//        if (it->first->process == 2 && it->first->virtual_page == 57)
//            cout << "stop" << endl;
        it->second.grow(it->first->referenced);
        it->first->referenced = 0;
    }
    // remap the recalculated age
    Pte *pte;
    for (int i = 0; i < frame_number; ++i) {
        Frame *fp = &frames[i];
        if (frame_table.find(fp) != frame_table.end()) {
            pte = frame_table[fp].first;
            ages[i] = age_table[pte];
        }
    }
}


void Pager::give_birth(std::unordered_map<int, Pte> &page_table) {
    for (auto it = page_table.begin(); it != page_table.end(); ++it) {
//        age_table[&it->second] = Age(0);
        it->second.referenced = 0;
        age_table[&it->second] = Age(it->second.referenced);
    }
}

void Pager::print_age() {
    for (int i = 0; i < frame_number; ++i) {
        Frame *fp = &frames[i];
        Pte *pte;
        pte = frame_table[fp].first;
        cout << i << ":" << ages[i].age << " R:" << pte->referenced << endl;
    }
}

