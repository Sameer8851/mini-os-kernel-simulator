#include "scheduler.hpp"
#include <iostream>
#include <limits.h>
#include <algorithm>
using namespace std;


// Constructor
Scheduler::Scheduler(SchedulingPolicy policy, int time_quantum)
    : policy(policy), time_quantum(time_quantum) {}

// Add process to ready queue
void Scheduler::addProcess(ProcessControlBlock* pcb) {
    pcb->state = ProcessState::READY;
    ready_queue.push_back(pcb);
}

void Scheduler::run() {
    cout << "\n--- Starting Scheduler ---\n";
    switch (policy) {
        case SchedulingPolicy::ROUND_ROBIN:
            runRoundRobin();
            break;
        case SchedulingPolicy::PRIORITY:
            runPriority();
            break;
        case SchedulingPolicy::SJF:
            runSJF();
            break;
    
    }

    cout << "--- Scheduler finished. All processes complete. ---\n";
}


void Scheduler::runRoundRobin() {
    int current_time = 0;

    while (!ready_queue.empty()) {
        // 1. Get the next process from the front of the queue
        ProcessControlBlock* current_pcb = ready_queue.front();
        ready_queue.erase(ready_queue.begin()); // Dequeue

        current_pcb->state = ProcessState::RUNNING;

        // 2. Determine the time slice for this turn
        int time_slice = std::min(time_quantum, current_pcb->remaining_burst_time);
        
        std::cout << "Time " << current_time << ": Running Process " << current_pcb->process_id 
                  << " for " << time_slice << " units.\n";

        // 3. Simulate running the process
        current_pcb->remaining_burst_time -= time_slice;
        current_time += time_slice;

        // 4. Check if the process has finished
        if (current_pcb->remaining_burst_time > 0) {
            // Not finished, move it to the back of the queue
            current_pcb->state = ProcessState::READY;
            ready_queue.push_back(current_pcb); // Enqueue at the back
        } else {
            // Finished, terminate it
            current_pcb->state = ProcessState::TERMINATED;
            std::cout << "Time " << current_time << ": Process " << current_pcb->process_id << " finished.\n";
        }
    }
}


void Scheduler::runPriority(){
    int current_time = 0;

    while(!ready_queue.empty()){
        // 1. Find the process with the highest priority
        int highest_priority = INT_MAX;
        int best_process_index = -1;

        for(size_t i = 0; i < ready_queue.size(); i++){
            if (ready_queue[i]->priority < highest_priority){
                highest_priority = ready_queue[i]->priority;
                best_process_index = i;
            }
        }

        // 2. Get the chosen process
        ProcessControlBlock* current_pcb = ready_queue[best_process_index];
        current_pcb->state = ProcessState::RUNNING;

        // 3. Simulate running the process to completion(non-preemptive)
        int burst_time = current_pcb->remaining_burst_time;
        cout << "Time " << current_time << ": Running Process " << current_pcb->process_id << " (Priority " << current_pcb->priority << ") for " << burst_time << " units.\n";
        current_time += burst_time;
        current_pcb-> remaining_burst_time = 0;
        current_pcb->state = ProcessState::TERMINATED;

        cout << "Time " << current_time << ": Process " << current_pcb->process_id << " finished.\n";

        // 4. Remove the finished process from the ready queue
        ready_queue.erase(ready_queue.begin()+best_process_index);
    }
}

void Scheduler::runSJF(){
    int current_time = 0;

    while(!ready_queue.empty()){
        // 1. find the process with the shortest burst time
        auto shortest_job_it = min_element(ready_queue.begin(),ready_queue.end(),[](const ProcessControlBlock* a,const ProcessControlBlock* b){
            return a->remaining_burst_time < b->remaining_burst_time;
        }
    );
    
    // 2. Get the chosen Process
    ProcessControlBlock* current_pcb = *shortest_job_it;
    current_pcb->state = ProcessState::RUNNING;

    // 3. Simulate running the process to completion
    int burst_time = current_pcb->remaining_burst_time;
    cout << "Time " << current_time << ": Running Process " << current_pcb->process_id 
                  << " (Burst " << burst_time << ") for " << burst_time << " units.\n";

    current_time += burst_time;
    current_pcb->remaining_burst_time =0;
    current_pcb->state = ProcessState::TERMINATED;

    cout << "Time " << current_time << ": Process " << current_pcb->process_id << " finished.\n";

    // 4. Remove the finished process from the ready queue
    ready_queue.erase(shortest_job_it);
    }
}