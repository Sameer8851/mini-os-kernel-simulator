#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "memory/virtual_memory/virtual_memory.hpp"
#include "scheduler/scheduler.hpp"
#include <map>

class System{
    public:
        System();
        void runCLI();
    private:
        // --- Core OS Components ---
        VirtualMemoryManager mmu;
        Scheduler scheduler;

        // --- Process Management ---
        std::map<int, ProcessControlBlock> process_table;
        int next_pid;

        // --- Private CLI Helper Functions ---
        void createProcess(int burst,int priority,int io_time,int io_freq);
        void accessMemory(int pid,int vpn , AccessType type);
        void showStats();
};

#endif