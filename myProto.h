#ifndef __MY_PROTO_H__
#define __MY_PROTO_H__

enum parts
{
    PART = 1000,
    TWO_PARTS = 2 * PART,
    FOUR_PARTS = 4 * PART
};

enum weight
{
    HEAVY = 10000
};

int test(int *data, int n);
int computeOnGPU(int *data, int n, int *counter);
double f(int i);

#endif
