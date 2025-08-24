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

    ProcessControlBlock(int id) : 
        process_id(id), 
        state(ProcessState::NEW) 
    {}
};

#endif