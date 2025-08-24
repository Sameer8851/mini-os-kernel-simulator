#include "../virtual_memory/virtual_memory.hpp"
#include <iostream>
#include <vector>
#include <utility>
#include <map>

using namespace std;

void runVirtualMemoryCLI()
{
    int memorySize, pageSize;
    cout << "=== Virtual Memory Manager ===\n";
    cout << "Enter total memory size (e.g., 64): ";
    cin >> memorySize;
    cout << "Enter page size (e.g., 4): ";
    cin >> pageSize;

    int policyChoice;
    cout << "\nSelect page replacement policy:\n";
    cout << "1. FIFO (First-In, First-Out)\n";
    cout << "2. LRU (Least Recently Used)\n";
    cout << "3. Clock (Second-Chance)\n";
    cout << "Choice: ";
    cin >> policyChoice;

    ReplacementPolicy policy;
    switch (policyChoice) {
        case 1:
            policy = ReplacementPolicy::FIFO;
            break;
        case 2:
            policy = ReplacementPolicy::LRU;
            break;
        case 3:
            policy = ReplacementPolicy::CLOCK;
            break;
        default:
            cout << "Invalid policy choice. Exiting.\n";
            return;
    }
    

    // instance for interactive policies
    VirtualMemoryManager vmm(memorySize, pageSize, policy);

    cout << "\nSelect Log Level for this session:\n";
    cout << "1. Normal (default)\n";
    cout << "2. Verbose (show steps)\n";
    cout << "3. Debug (show addresses and extra detail)\n";
    cout << "Choice: ";
    int logLevelChoice;
    cin >> logLevelChoice;
    
    switch(logLevelChoice) {
        case 2:
            vmm.setLogLevel(VERBOSE);
            break;
        case 3:
            vmm.setLogLevel(DEBUG);
            break;
        default:
            vmm.setLogLevel(NORMAL);
            break;
    }
    map<int,ProcessControlBlock> process_list;

    cout << "\nStarting interactive session...\n";

    while (true)
    {
        cout << "\n===== Virtual Memory Manager Menu =====\n";
        cout << "1. Allocate Process\n";
        cout << "2. Access Page\n";
        cout << "3. Free Process\n";
        cout << "4. Print Page Table\n";
        cout << "5. Print Frame Table\n";
        cout << "6. Total page faults\n";
        cout << "7. Exit\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            int pid;
            cout << "Enter Process ID: ";
            cin >> pid;
            if(process_list.count(pid)){
                cout << "Error: Process " << pid << " already exists.\n";
            }
            else{
                process_list.emplace(pid,pid);
                ProcessControlBlock& new_pcb = process_list.at(pid);
                vmm.allocateProcess(new_pcb);
            }
            
            break;
        }

        case 2: // Access Page
        {
            int pid, vpn;
            AccessType access_type;
            int type_choice;

            cout << "Enter Process ID: "; cin >> pid;
            cout << "Enter Virtual Page Number: "; cin >> vpn;
            cout << "Enter Access Type (1=READ, 2=WRITE, 3=EXECUTE): "; cin >> type_choice;
            
            switch(type_choice) {
                case 2: access_type = AccessType::WRITE; break;
                case 3: access_type = AccessType::EXECUTE; break;
                default: access_type = AccessType::READ; break;
            }

            if (process_list.count(pid)) {
                vmm.accessPage(process_list.at(pid), vpn, access_type);
            } else {
                cout << "Error: Process " << pid << " not found.\n";
            }
            break;
        }

        case 3: // Free Process
        {
            int pid;
            cout << "Enter Process ID to free: "; cin >> pid;
            if (process_list.count(pid)) {
                // Pass the PCB to the VMM to free its memory
                vmm.freeProcess(process_list.at(pid));
                // Then remove the process from our list
                process_list.erase(pid);
                cout << "Process " << pid << " has been freed.\n";
            } else {
                cout << "Error: Process " << pid << " not found.\n";
            }
            break;
        }

        case 4: // Print Page Table
        {
            int pid;
            cout << "Enter Process ID to view its page table: "; cin >> pid;
            if (process_list.count(pid)) {
                vmm.printPageTable(process_list.at(pid));
            } else {
                cout << "Error: Process " << pid << " not found.\n";
            }
            break;
        }

        case 5:
            vmm.printFrameTable();
            break;

        case 6:
            cout << "Total page faults so far: " << vmm.getPageFaults() << "\n";
            break;

        case 7:
            cout << "Exiting Virtual Memory Manager CLI...\n";
            return;

        default:
            cout << "Invalid choice. Try again.\n";
        }
    }
}
