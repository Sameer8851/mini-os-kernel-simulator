#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP


#include <string>
#include <vector>
#include "pcb.hpp"
#include "core/types.hpp" 
enum LogLevel;

enum class SchedulingPolicy {
    ROUND_ROBIN,
    PRIORITY,
    SJF
};


class Scheduler {
public:
    Scheduler(SchedulingPolicy policy,int time_quantum = 4);

    void run(std::vector<ProcessControlBlock*>& ready_queue, 
             std::vector<ProcessControlBlock*>& waiting_queue,
             int& system_time, 
             int num_steps = -1);

    void setLogLevel(LogLevel level);

private:
    
    SchedulingPolicy policy;
    int time_quantum;

    LogLevel current_log_level;
    void log(LogLevel level, const std::string& message);

};



#endif