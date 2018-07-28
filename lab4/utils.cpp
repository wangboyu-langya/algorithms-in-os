//
// Created by temporary on 2018/6/27.
//

#include "utils.h"


Request::Request(int rid, int a, int t) {
    arrival = a;
    track = t;
    id = rid;
    submit = -1;
    start = -1;
    finish = -1;
}
