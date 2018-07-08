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
    // required five fields, 5 bits
    int valid:1;
    int write_protect:1;
    int modified:1;
    int referenced:1;
    int page_out:1;
    // frame number, 7 bits
    int frame:7;
    // additional customized fields
    int file_map:1;
    int init:1;

    Pte() = default;
    Pte(int v, int w, int m, int r, int p, int f, int fm);

    void reset();
};

struct Instruction {
    Ins type;
    int location;
    Instruction(Ins t, int v);
};

struct Frame {
    int number;
    int process = -1;
    int virtual_page = 0;
    Frame(int n);;
};

// load a random number file
void load_rand(string &rand_name);

// return a random number
int my_random(int burst);

#endif //LAB02_UTILS_H
