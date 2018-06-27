//
// Created by temporary on 2018/6/26.
//

#ifndef LAB02_UTILS_H
#define LAB02_UTILS_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
// enum types
// scheduler type
enum sch {
    FCFS, LCFS, SJF, RR, PRIO
};
enum state {
    Created, Ready, Running, Blocked, Done, Empty
};

vector<int> rand_v; // random vector
vector<int>::iterator rand_it;

void load_rand(string &rand_name) {
    // read random number into an array

    std::ifstream rfs(rand_name);
    if (!rfs) {
        cout << "Cannot open random file.\n";
        exit(1);
    }
    // temporary variables to read random number
    string rand_s;
    rfs >> rand_s;
    int len = stoi(rand_s);
    while (rfs >> rand_s)
        rand_v.push_back(stoi(rand_s));
    // make sure this process is right
    if (rand_v.size() != len) {
        cerr << "Error reading random file, the number doesn't match.\n";
        exit(1);
    }
    rand_it = rand_v.begin();
}

int my_random(int burst) {
    // use vector iterator, if reaches the end, reload the iterator
    if (rand_it == rand_v.end())
        rand_it = rand_v.begin();
    return *rand_it++ % burst + 1;
}

#endif //LAB02_UTILS_H
