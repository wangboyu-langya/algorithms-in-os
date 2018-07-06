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
#include "Process.h"

using namespace std;
// enum type
enum Pge {
    Fifo, Sec_chance, Random, Nru, Clock, Aging
};
enum Instruct {
    r, w, c
};

// struct
struct Pte {
    // required five fields
    unsigned int valid:1;
    unsigned int write_protect:1;
    unsigned int modified:1;
    unsigned int referenced:1;
    unsigned int page_out:1;
    // frame number
    unsigned int frame:7;
    // additional customized fields
    unsigned int file_map:1;

    Pte() = default;
    Pte(int v, int w, int m, int r, int p, int f, int fm);
};

struct Ins {
    Instruct type;
    int virtual_page;
};

struct frame {
    unsigned int number;
};

// load a random number file
void load_rand(string &rand_name);

// return a random number
int my_random(int burst);

#endif //LAB02_UTILS_H
