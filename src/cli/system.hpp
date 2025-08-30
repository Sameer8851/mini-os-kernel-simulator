#ifndef SYSTEM_HPP
#define SYSTEM_HPP


#include <string>
#include "memory/virtual_memory/virtual_memory.hpp"
#include "scheduler/scheduler.hpp"
#include <map>


std::string processStateToString(ProcessState state);


class System{
    public:
        System();
        void runCLI();

        void setLogLevel(LogLevel level);
    private:
        // --- Core OS Components ---
        VirtualMemoryManager mmu;
        Scheduler scheduler;

        // --- Process Management ---
        std::map<int, ProcessControlBlock> process_table;
        int next_pid;
        int system_time;

        std::vector<ProcessControlBlock*> ready_queue;
        std::vector<ProcessControlBlock*> waiting_queue;


        // --- Statistics Tracking ---
        int total_processes_created;
        int total_turnaround_time;
        int finished_process_count;

        // --- Private CLI Helper Functions ---
        void createProcess(int burst,int priority,int io_time,int io_freq);
        void accessMemory(int pid,int vpn , AccessType type);
        void showStats();
        void showProcessList();
        void setSystemLogLevel(LogLevel level);
};

#endif