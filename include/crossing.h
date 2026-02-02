#ifndef CROSSING_H
#define CROSSING_H

#include <vector>

//Represents a single crossing 
struct Crossing {
    int index;      //crossing number
    int sign;       // +1 or -1

    Crossing(int i, int s) : index(i), sign(s) {}

    //comparing crossing for memorisation
    bool operator==(const Crossing& other) const {
        return index == other.index && sign == other.sign;
    }
};

#endif