#include "scheduler/scheduler.hpp"
#include <iostream>
#include <vector>

// --- Test Suites ---

void testRoundRobin() {
    std::cout << "\n=============================================\n";
    std::cout << "  Testing Scheduler: Round Robin (Quantum=4)\n";
    std::cout << "=============================================\n";

    Scheduler scheduler(SchedulingPolicy::ROUND_ROBIN, 4);

    ProcessControlBlock p1(1, 10, 0); // ID=1, Burst=10
    ProcessControlBlock p2(2, 7, 0);  // ID=2, Burst=7
    ProcessControlBlock p3(3, 4, 0);  // ID=3, Burst=4

    scheduler.addProcess(&p1);
    scheduler.addProcess(&p2);
    scheduler.addProcess(&p3);
    scheduler.run();
}

void testPriority() {
    std::cout << "\n=============================================\n";
    std::cout << "  Testing Scheduler: Priority (Non-Preemptive)\n";
    std::cout << "=============================================\n";

    Scheduler scheduler(SchedulingPolicy::PRIORITY);

    // Note: Lower number = higher priority
    ProcessControlBlock p1(1, 10, 2); // ID=1, Burst=10, Prio=2
    ProcessControlBlock p2(2, 7, 1);  // ID=2, Burst=7,  Prio=1 (Highest)
    ProcessControlBlock p3(3, 4, 3);  // ID=3, Burst=4,  Prio=3

    scheduler.addProcess(&p1);
    scheduler.addProcess(&p2);
    scheduler.addProcess(&p3);
    scheduler.run(); // Should run in order: P2 -> P1 -> P3
}

void testSJF() {
    std::cout << "\n=============================================\n";
    std::cout << "  Testing Scheduler: SJF (Non-Preemptive)\n";
    std::cout << "=============================================\n";

    Scheduler scheduler(SchedulingPolicy::SJF);

    ProcessControlBlock p1(1, 10, 0); // ID=1, Burst=10
    ProcessControlBlock p2(2, 7, 0);  // ID=2, Burst=7
    ProcessControlBlock p3(3, 4, 0);  // ID=3, Burst=4 (Shortest)

    scheduler.addProcess(&p1);
    scheduler.addProcess(&p2);
    scheduler.addProcess(&p3);
    scheduler.run(); // Should run in order: P3 -> P2 -> P1
}


// --- Test Runner Main Function ---

int main() {
    std::cout << "===== Running All Scheduler Tests =====\n";
    
    testRoundRobin();
    testPriority();
    testSJF();

    std::cout << "\n===== All Scheduler Tests Completed =====\n";
    return 0;
}