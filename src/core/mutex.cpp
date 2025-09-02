#include "mutex.hpp"


bool Mutex::lock(ProcessControlBlock* pcb){
    if(!locked){
        // if not locked, acquire it
        locked = true;
        owner = pcb;
        return true;
    } else {
        pcb->state = ProcessState::BLOCKED_ON_MUTEX;
        waiting_processes.push_back(pcb);
        return false;
    }
}

ProcessControlBlock* Mutex::unlock(ProcessControlBlock* pcb){
    if(!locked || owner != pcb){
        return nullptr;
    }

    if(!waiting_processes.empty()){
        ProcessControlBlock* next_pcb = waiting_processes.front();
        waiting_processes.erase(waiting_processes.begin());

        next_pcb->state = ProcessState::READY;
        owner = next_pcb;
        return next_pcb;
    } else {
        locked = false;
        owner = nullptr;
        return nullptr;
    }
}