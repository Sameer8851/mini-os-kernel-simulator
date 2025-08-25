#include "scheduler.hpp"
#include <iostream>

Scheduler::Scheduler(int time_quantum) : time_quantum(time_quantum) {
    std::cout << "Round Robin Scheduler initialized with a time quantum of " << time_quantum << " units.\n";
}

void Scheduler::addProcess(ProcessControlBlock* pcb) {
    pcb->state = ProcessState::READY;
    ready_queue.push(pcb);
    std::cout << "Process " << pcb->process_id << " added to the ready queue.\n";
}

void Scheduler::run() {
    std::cout << "\n--- Starting Scheduler ---\n";
    int current_time = 0;

    while (!ready_queue.empty()) {
        // 1. Get the next process from the front of the queue
        ProcessControlBlock* current_pcb = ready_queue.front();
        ready_queue.pop();

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
            ready_queue.push(current_pcb);
        } else {
            // Finished, terminate it
            current_pcb->state = ProcessState::TERMINATED;
            std::cout << "Time " << current_time << ": Process " << current_pcb->process_id << " finished.\n";
        }
    }

    std::cout << "--- Scheduler finished. All processes complete. ---\n";
}