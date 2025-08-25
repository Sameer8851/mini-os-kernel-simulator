#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <queue>
#include "pcb.hpp"

class Scheduler {
public:
    Scheduler(int time_quantum);

    void addProcess(ProcessControlBlock* pcb);
    void run();

private:
    int time_quantum;
    std::queue<ProcessControlBlock*> ready_queue;
};

#endif