#ifndef UTILS_H
#define UTILS_H

struct msg {
    float T;
    int Index;
};

struct msg prepare_message(int i_Index, float i_Temperature);

#endif
