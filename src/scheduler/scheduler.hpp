#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <queue>
#include "pcb.hpp"


enum class SchedulingPolicy {
    ROUND_ROBIN,
    PRIORITY,
    SJF
};


class Scheduler {
public:
    Scheduler(SchedulingPolicy policy,int time_quantum = 4);

    void addProcess(ProcessControlBlock* pcb);
    void run();

private:
    void runRoundRobin();
    void runPriority();
    void runSJF();

    SchedulingPolicy policy;
    int time_quantum;
    std::vector<ProcessControlBlock*> ready_queue;
};



#endif