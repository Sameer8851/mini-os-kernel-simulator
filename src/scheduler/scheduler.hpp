// This is a header guard
#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP
using namespace std;


#include <string>

struct Process
{
    string name;
    int burstTime;
    int remainingTime;
};

void runScheduler(); //Declaration of the function that runs our Round-Robin scheduling logic.

//closes the header guard

#endif