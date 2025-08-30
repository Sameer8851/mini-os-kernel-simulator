#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <queue>
#include "pcb.hpp"
#include <vector>


enum class SchedulingPolicy {
    ROUND_ROBIN,
    PRIORITY,
    SJF
};


class Scheduler {
public:
    Scheduler(SchedulingPolicy policy,int time_quantum = 4);

    void addProcess(ProcessControlBlock* pcb);
    void run(int num_steps = -1);

private:
    void runRoundRobin();
    void runPriority();
    void runSJF();
    std::vector<ProcessControlBlock*> waiting_queue;

    SchedulingPolicy policy;
    int time_quantum;
    std::vector<ProcessControlBlock*> ready_queue;
};



#endif