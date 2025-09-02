#ifndef MUTEX_HPP
#define MUTEX_HPP

#include <vector>
#include "../scheduler/pcb.hpp"

class Mutex {
    public:
        Mutex() : locked(false), owner(nullptr) {}

        bool lock(ProcessControlBlock* pcb);
        ProcessControlBlock* unlock(ProcessControlBlock* pcb);

    private:
        bool locked;
        ProcessControlBlock* owner;
        std::vector<ProcessControlBlock*> waiting_processes;
};

#endif