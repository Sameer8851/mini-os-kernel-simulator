#include "scheduler.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <limits>
#include "../memory/virtual_memory/virtual_memory.hpp"
using namespace std;

Scheduler::Scheduler(SchedulingPolicy policy, int time_quantum) 
    : policy(policy), time_quantum(time_quantum), current_log_level(NORMAL) 
{
    std::string policy_name;
    switch (policy) {
        case SchedulingPolicy::ROUND_ROBIN: policy_name = "Round Robin"; break;
        case SchedulingPolicy::PRIORITY: policy_name = "Priority"; break;
        case SchedulingPolicy::SJF: policy_name = "SJF"; break;
    }
    log(NORMAL, "Scheduler initialized for policy: " + policy_name);
}

void Scheduler::setLogLevel(LogLevel level) {
    current_log_level = level;
}

void Scheduler::log(LogLevel level, const std::string& message) {
    if (current_log_level >= level) {
        std::cout << message << std::endl;
    }
}





// --- RUN METHOD --
void Scheduler::run(std::vector<ProcessControlBlock*>& ready_queue, 
    std::vector<ProcessControlBlock*>& waiting_queue,int& system_time, 
    int num_steps) {
    log(NORMAL, "\n--- Starting Scheduler ---");
    
    ProcessControlBlock* current_process = nullptr;
    int time_in_quantum = 0;
    int steps_taken =0;

    if (!ready_queue.empty() && ready_queue.front()->state == ProcessState::RUNNING) {
        current_process = ready_queue.front();
        ready_queue.erase(ready_queue.begin());
    }

    // Main simulation loop: runs as long as there are processes to manage
    while (true) {
        if(num_steps != -1 && steps_taken >= num_steps){
            log(NORMAL, "--- Scheduler paused after " + std::to_string(steps_taken) + " steps. ---");
            if (current_process) { 
                current_process->state = ProcessState::READY;
                ready_queue.insert(ready_queue.begin(), current_process); 
            }
            break;
        }
        
        // 1. Check waiting queue and move any finished I/O processes to the ready queue
        for (size_t i = 0; i < waiting_queue.size(); ) {
            ProcessControlBlock* pcb = waiting_queue[i];
            pcb->io_burst_time--;
            if (pcb->io_burst_time <= 0) {
                pcb->state = ProcessState::READY;
                ready_queue.push_back(pcb);
                waiting_queue.erase(waiting_queue.begin() + i);
                log(VERBOSE, "Time " + std::to_string(system_time) + ": P" + std::to_string(pcb->process_id) + " finished I/O, moved to ready.");
            } else {
                i++;
            }
        }

        // 2. If a process is running, check if it needs to stop
        if (current_process != nullptr) {
            bool should_stop = false;
            if (current_process->remaining_burst_time <= 0) { // Process finished
                current_process->state = ProcessState::TERMINATED;
                current_process->completion_time = system_time;
                log(VERBOSE, "Time " + std::to_string(system_time) + ": P" + std::to_string(current_process->process_id) + " finished.");
                should_stop = true;
            } else if (current_process->io_burst_frequency > 0 && current_process->time_since_last_io >= current_process->io_burst_frequency) { // Process needs I/O
                current_process->state = ProcessState::WAITING;
                current_process->time_since_last_io = 0;
                waiting_queue.push_back(current_process);
                log(VERBOSE, "Time " + std::to_string(system_time) + ": P" + std::to_string(current_process->process_id) + " moved to waiting for I/O.");
                should_stop = true;
            } else if (policy == SchedulingPolicy::ROUND_ROBIN && time_in_quantum >= time_quantum) { // Quantum expired for RR
                current_process->state = ProcessState::READY;
                ready_queue.push_back(current_process);
                log(VERBOSE, "Time " + std::to_string(system_time) + ": P" + std::to_string(current_process->process_id) + " preempted (quantum expired).");
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
                auto it = min_element(ready_queue.begin(), ready_queue.end(),
                    [](const auto* a, const auto* b){ return a->priority < b->priority; });
                current_process = *it;
                ready_queue.erase(it);
            } else if (policy == SchedulingPolicy::SJF) {
                auto it = min_element(ready_queue.begin(), ready_queue.end(),
                    [](const auto* a, const auto* b){ return a->remaining_burst_time < b->remaining_burst_time; });
                current_process = *it;
                ready_queue.erase(it);
            }
            current_process->state = ProcessState::RUNNING;
        }

        // 4. If a process is running, simulate one time unit of work
        if (current_process != nullptr) {
            std::stringstream ss;
            ss << "Time " << system_time << ": Running P" << current_process->process_id << ". "
               << "(CPU Burst left: " << current_process->remaining_burst_time << ")";
            log(DEBUG, ss.str());

            current_process->remaining_burst_time--;
            current_process->time_since_last_io++;
            time_in_quantum++;
        } else {
            log(DEBUG, "Time " + std::to_string(system_time) + ": CPU is idle.");
        }

        // 5. Check if the simulation is complete
        if (ready_queue.empty() && waiting_queue.empty() && current_process == nullptr) {
            log(NORMAL, "--- Scheduler finished. All processes complete. ---");
            break; // Exit the main loop
        }
        
        // 6. Advance the simulation time and step count
        system_time++;
        steps_taken++;
    }
}