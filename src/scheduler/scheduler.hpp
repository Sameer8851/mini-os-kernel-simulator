#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <queue>
#include <string>
#include <vector>
#include "pcb.hpp"
#include "memory/virtual_memory/virtual_memory.hpp" 
enum LogLevel;

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

    void setLogLevel(LogLevel level);

private:
    void runRoundRobin();
    void runPriority();
    void runSJF();
    std::vector<ProcessControlBlock*> waiting_queue;

    SchedulingPolicy policy;
    int time_quantum;
    std::vector<ProcessControlBlock*> ready_queue;

    LogLevel current_log_level;
    void log(LogLevel level, const std::string& message);

};



#endif