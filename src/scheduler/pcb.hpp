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
    int priority;

    // I/O properties
    int io_burst_time;
    int io_burst_frequency;
    int time_since_last_io;

    ProcessControlBlock(int id,int burst_time,int prio,int io_time = 0,int io_freq = 0) : 
        process_id(id), 
        state(ProcessState::NEW),
        remaining_burst_time(burst_time),
        priority(prio),
        io_burst_time(io_time),
        io_burst_frequency(io_freq),
        time_since_last_io(0)
    {}
};

#endif