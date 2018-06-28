//
// Created by temporary on 2018/6/26.
//

#ifndef LAB02_DES_H
#define LAB02_DES_H

#include <list>
#include "Event.h"

using namespace std;

class Des {
public:
    // with vector insert takes O(n), search takes O(lgn)
    // with list insert takes O(1), search takes O(n)
    // so basically they are the same
    // here I choose list sice this is easier to implement
    list<Event> events;

    // insert an event in ascending order
    void insert(Event e);

    // return the first element of the list and delete it
    // TODO: need to check the reference is valid
    Event pop();

    // whether other events at the same time
    bool next(int time);
};


#endif //LAB02_DES_H
