#include "system.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

System::System() :
    mmu(128,4,ReplacementPolicy::LRU),
    scheduler(SchedulingPolicy::ROUND_ROBIN,4),
    next_pid(1)
{
    cout << "System initialized.\n";
}

// Main interactive loop for the unified CLI
void System::runCLI(){
    string line,command;
    cout << "\nWelcome to the MOSKS Unified CLI.\nType 'help' for a list of commands.\n";

    while (true){
        cout << "\nMOSKS> ";
        getline(cin,line);
        istringstream iss(line);
        iss >> command;

        if(command == "help"){
            std::cout << "Available Commands:\n"
                      << "  create <burst> <prio> [io_time] [io_freq] - Create a new process.\n"
                      << "  access <pid> <vpn> <type>                 - Access memory (type: READ, WRITE, EXECUTE).\n"
                      << "  run                                       - Run the CPU scheduler.\n"
                      << "  ps                                        - Show process list.\n"
                      << "  mem <pid>                                 - Show page table for a process.\n"
                      << "  stats                                     - Show system statistics.\n"
                      << "  exit                                      - Exit the simulator.\n";
        } else if (command == "create"){
            int burst = 0, prio = 0, io_time = 0, io_freq = 0;
            iss >> burst >> prio >> io_time >> io_freq;
            if(burst > 0){
                createProcess(burst,prio,io_time,io_freq);
            }
            else{
                cout << "Usage: create <burst_time> <priority> [io_time] [io_freq]\n";
            }
        } else if (command == "access"){
            int pid = 0, vpn = 0;
            string type_str;
            iss >> pid >> vpn >> type_str;
            AccessType type = AccessType::READ;
            if (type_str == "WRITE") type = AccessType::WRITE;
            if (type_str == "EXECUTE") type = AccessType::EXECUTE;
            accessMemory(pid,vpn,type);
        } else if(command == "run"){
            scheduler.run();
        } else if(command == "ps"){
            cout << "Process list functionality coming soon.\n";
        } else if(command == "mem"){
            int pid = 0;
            iss >> pid;
            if(process_table.count(pid)){
                mmu.printPageTable(process_table.at(pid));
            } else {
                cout << "Process " << pid << " not found.\n";
            }
        } else if (command == "stats"){
            showStats();
        }else if (command == "exit"){
            cout << "Shutting down MOSKS...\n";
            break;
        } else if (!command.empty()){
            cout << "Unknown command: '" << command << "'. Type 'help' for a list of commands.\n";
        }
    }
}

// --- Private Helper Functions ---

void System::createProcess(int burst, int priority, int io_time, int io_freq) {
    // 1. Create and store the PCB
    process_table.emplace(next_pid, ProcessControlBlock(next_pid, burst, priority, io_time, io_freq));
    ProcessControlBlock& new_pcb = process_table.at(next_pid);

    // 2. Initialize its memory with the MMU
    mmu.allocateProcess(new_pcb);

    // 3. Add it to the scheduler's ready queue
    scheduler.addProcess(&new_pcb);

    std::cout << "Created Process " << next_pid << ".\n";
    next_pid++;
}

void System::accessMemory(int pid, int vpn, AccessType type) {
    if (process_table.count(pid)) {
        mmu.accessPage(process_table.at(pid), vpn, type);
    } else {
        std::cout << "Process " << pid << " not found.\n";
    }
}

void System::showStats() {
    std::cout << "--- System Statistics ---\n";
    std::cout << "Total Page Faults: " << mmu.getPageFaults() << "\n";
    mmu.printFrameTable();
}
