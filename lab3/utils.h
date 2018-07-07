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
// enum type
enum Pge {
    Fifo, Sec_chance, Random, Nru, Clock, Aging
};
enum Ins{
    r, w, c
};

// struct
struct Pte {
    // required five fields
    unsigned valid:1;
    unsigned write_protect:1;
    unsigned modified:1;
    unsigned referenced:1;
    unsigned page_out:1;
    // frame number
    unsigned frame:7;
    // additional customized fields
    unsigned file_map:1;
    unsigned init:1;

    Pte() = default;
    Pte(unsigned v, unsigned w, unsigned m, unsigned r, unsigned p, unsigned f, unsigned fm);
};

struct Instruction {
    Ins type;
    int virtual_page;
    Instruction(Ins t, int v);
};

struct Frame {
    unsigned int number;
};

// load a random number file
void load_rand(string &rand_name);

// return a random number
int my_random(int burst);

#endif //LAB02_UTILS_H
