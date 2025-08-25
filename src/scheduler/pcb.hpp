#ifndef PCB_HPP
#define PCB_HPP

#include "../memory/virtual_memory/memory_types.hpp" // Include the new types file

enum class ProcessState {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
};

struct ProcessControlBlock {
    int process_id;
    ProcessState state;
    PageDirectory page_directory; 
    int remaining_burst_time;

    ProcessControlBlock(int id,int burst_time) : 
        process_id(id), 
        state(ProcessState::NEW),
        remaining_burst_time(burst_time) 
    {}
};

#endif