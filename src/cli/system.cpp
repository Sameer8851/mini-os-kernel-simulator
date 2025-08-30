#include "system.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

System::System() : mmu(128, 4, ReplacementPolicy::LRU),
                   scheduler(SchedulingPolicy::ROUND_ROBIN, 4),
                   next_pid(1),
                   total_processes_created(0),
                   total_turnaround_time(0),
                   finished_process_count(0)
{
    cout << "System initialized.\n";
}

// Main interactive loop for the unified CLI
void System::runCLI()
{
    string line, command;
    cout << "\nWelcome to the MOSKS Unified CLI.\nType 'help' for a list of commands.\n";

    while (true)
    {
        cout << "\nMOSKS> ";
        getline(cin, line);
        istringstream iss(line);
        iss >> command;

        if (command == "help")
        {
            std::cout << "Available Commands:\n"
                      << "  loglevel <level>                           - Set system log level (0=NORMAL, 1=VERBOSE, 2=DEBUG).\n"
                      << "  create <burst> <prio> [io_time] [io_freq]  - Create a new process.\n"
                      << "  access <pid> <vpn> <type>                  - Access memory (type: READ, WRITE, EXECUTE).\n"
                      << "  run [steps]                                - Run the CPU scheduler.\n"
                      << "  ps                                         - Show process list.\n"
                      << "  mem <pid>                                  - Show page table for a process.\n"
                      << "  stats                                      - Show system statistics.\n"
                      << "  exit                                       - Exit the simulator.\n";
        }
        else if (command == "create")
        {
            int burst = 0, prio = 0, io_time = 0, io_freq = 0;
            iss >> burst >> prio >> io_time >> io_freq;
            if (burst > 0)
            {
                createProcess(burst, prio, io_time, io_freq);
            }
            else
            {
                cout << "Usage: create <burst_time> <priority> [io_time] [io_freq]\n";
            }
        }
        else if (command == "access")
        {
            int pid = 0, vpn = 0;
            string type_str;
            iss >> pid >> vpn >> type_str;
            AccessType type = AccessType::READ;
            if (type_str == "WRITE")
                type = AccessType::WRITE;
            if (type_str == "EXECUTE")
                type = AccessType::EXECUTE;
            accessMemory(pid, vpn, type);
        }
        else if (command == "run")
        {
            int num_steps = -1; // Default to run until competion
            iss >> num_steps;

            if (num_steps > 0)
            {
                cout << "Running scheduler for " << num_steps << " steps...\n";
                scheduler.run(ready_queue, waiting_queue, system_time, num_steps);
            }
            else
            {
                cout << "Running scheduler until all processes complete...\n";
                scheduler.run(ready_queue, waiting_queue, system_time);
            }
        }
        else if (command == "ps")
        {
            showProcessList();
        }
        else if (command == "mem")
        {
            int pid = 0;
            iss >> pid;
            if (process_table.count(pid))
            {
                mmu.printPageTable(process_table.at(pid));
            }
            else
            {
                cout << "Process " << pid << " not found.\n";
            }
        }
        else if (command == "stats")
        {
            showStats();
        } else if( command == "loglevel"){
            int level = 0;
            iss >> level;
            switch(level) {
                case 1: setLogLevel(VERBOSE); break;
                case 2: setLogLevel(DEBUG); break;
                default: setLogLevel(NORMAL); break;
            }
        }
        else if (command == "exit")
        {
            cout << "Shutting down MOSKS...\n";
            break;
        }
        else if (!command.empty())
        {
            cout << "Unknown command: '" << command << "'. Type 'help' for a list of commands.\n";
        }
    }
}

// --- Private Helper Functions ---

void System::createProcess(int burst, int priority, int io_time, int io_freq)
{
    total_processes_created++;

    // 1. Create and store the PCB
    process_table.emplace(next_pid, ProcessControlBlock(next_pid, burst, priority, io_time, io_freq));
    ProcessControlBlock &new_pcb = process_table.at(next_pid);
    new_pcb.creation_time = system_time;
    // 2. Initialize its memory with the MMU
    mmu.allocateProcess(new_pcb);

    // 3. Add it to the scheduler's ready queue
    ready_queue.push_back(&new_pcb);

    std::cout << "Created Process " << next_pid << ".\n";
    next_pid++;
}

void System::accessMemory(int pid, int vpn, AccessType type)
{
    if (process_table.count(pid))
    {
        mmu.accessPage(process_table.at(pid), vpn, type);
    }
    else
    {
        std::cout << "Process " << pid << " not found.\n";
    }
}

void System::showStats()
{
    int ready_count = 0;
    int waiting_count = 0;
    int terminated_count = 0;
    int running_count = 0;

    int total_turnaround_time = 0;
    int finished_process_count = 0;

    for (const auto &pair : process_table)
    {
        const ProcessControlBlock &pcb = pair.second;
        switch (pcb.state) {
            case ProcessState::READY: ready_count++; break;
            case ProcessState::WAITING: waiting_count++; break;
            case ProcessState::RUNNING: running_count++; break;
            case ProcessState::TERMINATED: 
                terminated_count++;
                if (pcb.completion_time != -1) {
                    total_turnaround_time += (pcb.completion_time - pcb.creation_time);
                    finished_process_count++;
                }
                break;
            default: break;  
        }
    }

    double avg_turnaround_time = 0.0;
    if (finished_process_count > 0) {
        avg_turnaround_time = static_cast<double>(total_turnaround_time) / finished_process_count;
    }

    std::cout << "\n--- System Statistics ---\n";
    std::cout << "Current System Time: " << system_time << "\n";
    std::cout << "Total Processes Created: " << process_table.size() << "\n";
    if (finished_process_count > 0) {
        std::cout << "Average Turnaround Time: " << avg_turnaround_time << " units\n";
    }
    std::cout << "Process States:\n";
    std::cout << "  - Running:    " << running_count << "\n";
    std::cout << "  - Ready:      " << ready_count << "\n";
    std::cout << "  - Waiting:    " << waiting_count << "\n";
    std::cout << "  - Terminated: " << terminated_count << "\n";
    
    std::cout << "\n--- MMU Statistics ---\n";
    std::cout << "Total Page Faults: " << mmu.getPageFaults() << "\n";
    mmu.printFrameTable();
}

string processStateToString(ProcessState state)
{
    switch (state)
    {
    case ProcessState::NEW:
        return "NEW";
    case ProcessState::READY:
        return "READY";
    case ProcessState::RUNNING:
        return "RUNNING";
    case ProcessState::WAITING:
        return "WAITING";
    case ProcessState::TERMINATED:
        return "TERMINATED";
    default:
        return "UNKNOWN";
    }
}
void System::showProcessList()
{
    std::cout << "--- Process List ---\n";
    std::cout << std::left << std::setw(5) << "PID"
              << std::setw(12) << "State"
              << std::setw(10) << "Burst"
              << std::setw(10) << "Priority" << "\n";
    std::cout << "-------------------------------------\n";

    if (process_table.empty())
    {
        std::cout << "(No processes)\n";
        return;
    }

    for (const auto &pair : process_table)
    {
        const ProcessControlBlock &pcb = pair.second;
        std::cout << std::left << std::setw(5) << pcb.process_id
                  << std::setw(12) << processStateToString(pcb.state)
                  << std::setw(10) << pcb.remaining_burst_time
                  << std::setw(10) << pcb.priority << "\n";
    }
}

void System::setLogLevel(LogLevel level) {
    mmu.setLogLevel(level);
    scheduler.setLogLevel(level);
    std::cout << "System log level set.\n";
}