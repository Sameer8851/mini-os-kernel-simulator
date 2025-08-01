using namespace std;
#include "scheduler.hpp"
#include <iostream>
#include <queue>

void runScheduler(){
    queue<Process> q;

    q.push({"P1",6,6});
    q.push({"P2",4,4});
    q.push({"P3",9,9});

    const int quantum = 3;
    int time = 0;

    cout << "\n--- CPU Scheduler Simulation ---\n";

    while(!q.empty()){
        Process current = q.front();
        q.pop();

        cout << "[Time " << time << "] " << "Running " << current.name << "...\n";

        int execTime = min(current.remainingTime,quantum);
        current.remainingTime -=execTime;
        time += execTime;

        if(current.remainingTime > 0){
            q.push(current);
            cout << current.name << " not finished, re-queued with " << current.remainingTime << " units remaining.\n";
        }
        else {
            cout << current.name << " has completed execution \n";
        }
    }
    cout << "[Time" << time << "] CPU Scheduler finished!\n";

}
