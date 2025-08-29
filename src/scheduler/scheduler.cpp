#include "scheduler.hpp"
#include <iostream>
#include <algorithm>
#include <limits>    

Scheduler::Scheduler(SchedulingPolicy policy, int time_quantum) 
    : policy(policy), time_quantum(time_quantum) 
{
    std::cout << "Scheduler initialized for policy: " 
              << (policy == SchedulingPolicy::ROUND_ROBIN ? "Round Robin" :
                 (policy == SchedulingPolicy::PRIORITY ? "Priority" : "SJF"))
              << "\n";
}

void Scheduler::addProcess(ProcessControlBlock* pcb) {
    pcb->state = ProcessState::READY;
    ready_queue.push_back(pcb);
    std::cout << "Process " << pcb->process_id << " added to the ready queue.\n";
}

void Scheduler::run() {
    std::cout << "\n--- Starting Scheduler ---\n";
    int current_time = 0;
    ProcessControlBlock* current_process = nullptr;
    int time_in_quantum = 0;

    // Main simulation loop: runs as long as there are processes to manage
    while (true) {
        
        // 1. Check waiting queue and move any finished I/O processes to the ready queue
        for (size_t i = 0; i < waiting_queue.size(); ) {
            ProcessControlBlock* pcb = waiting_queue[i];
            pcb->io_burst_time--;
            if (pcb->io_burst_time <= 0) {
                pcb->state = ProcessState::READY;
                ready_queue.push_back(pcb);
                waiting_queue.erase(waiting_queue.begin() + i);
                std::cout << "Time " << current_time << ": P" << pcb->process_id << " finished I/O, moved to ready.\n";
            } else {
                ++i;
            }
        }

        // 2. If a process is running, check if it needs to stop
        if (current_process != nullptr) {
            bool should_stop = false;
            if (current_process->remaining_burst_time <= 0) { // Process finished
                current_process->state = ProcessState::TERMINATED;
                std::cout << "Time " << current_time << ": P" << current_process->process_id << " finished.\n";
                should_stop = true;
            } else if (current_process->io_burst_frequency > 0 && current_process->time_since_last_io >= current_process->io_burst_frequency) { // Process needs I/O
                current_process->state = ProcessState::WAITING;
                current_process->time_since_last_io = 0;
                waiting_queue.push_back(current_process);
                std::cout << "Time " << current_time << ": P" << current_process->process_id << " moved to waiting for I/O.\n";
                should_stop = true;
            } else if (policy == SchedulingPolicy::ROUND_ROBIN && time_in_quantum >= time_quantum) { // Quantum expired for RR
                current_process->state = ProcessState::READY;
                ready_queue.push_back(current_process);
                std::cout << "Time " << current_time << ": P" << current_process->process_id << " preempted (quantum expired).\n";
                should_stop = true;
            }

            if (should_stop) {
                current_process = nullptr;
                time_in_quantum = 0;
            }
        }

        // 3. If no process is running, select a new one from the ready queue based on policy
        if (current_process == nullptr && !ready_queue.empty()) {
            if (policy == SchedulingPolicy::ROUND_ROBIN) {
                current_process = ready_queue.front();
                ready_queue.erase(ready_queue.begin());
            } else if (policy == SchedulingPolicy::PRIORITY) {
                auto it = std::min_element(ready_queue.begin(), ready_queue.end(),
                    [](const auto* a, const auto* b){ return a->priority < b->priority; });
                current_process = *it;
                ready_queue.erase(it);
            } else if (policy == SchedulingPolicy::SJF) {
                auto it = std::min_element(ready_queue.begin(), ready_queue.end(),
                    [](const auto* a, const auto* b){ return a->remaining_burst_time < b->remaining_burst_time; });
                current_process = *it;
                ready_queue.erase(it);
            }
            current_process->state = ProcessState::RUNNING;
        }

        // 4. If a process is running, simulate one time unit of work
        if (current_process != nullptr) {
            std::cout << "Time " << current_time << ": Running P" << current_process->process_id << ".\n";
            current_process->remaining_burst_time--;
            current_process->time_since_last_io++;
            time_in_quantum++;
        } else {
            std::cout << "Time " << current_time << ": CPU is idle.\n";
        }

        // 5. Check if the simulation is complete
        if (ready_queue.empty() && waiting_queue.empty() && current_process == nullptr) {
            std::cout << "--- Scheduler finished. All processes complete. ---\n";
            break; // Exit the main loop
        }
        
        // 6. Advance the simulation time
        current_time++;
    }
}