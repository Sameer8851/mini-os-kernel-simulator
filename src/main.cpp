#include <iostream>
#include "scheduler/scheduler.hpp"
#include "memory/memory.hpp"
using namespace std;

void bootSystem(){
    cout << "Initializing Mini OS Kernel Simulator (MOSKS)..." << endl;

    cout << "--> Loading CPU Scheduler..." << endl;
    cout << "--> Loading Memory Manager..." << endl;
    cout << "--> Mounting File System..." << endl;
    cout << "--> Launching Shell Interface..." << endl;

    cout << "System Boot Complete [OK]" << endl;

}

int main(){
    bootSystem();
    runScheduler();
    runMemoryManager();

    return 0;
}