# Mini OS Kernel Simulator (MOSKS)

A comprehensive, text-based simulator for core operating system concepts built in C++. This project simulates a multi-process environment with a sophisticated memory management unit and a preemptive CPU scheduler.

---

## 🚀 Features

This simulator implements a wide range of OS features from the ground up:

### Core System
- **Unified CLI:** An interactive shell to manage the entire OS simulation.
- **Process Management:** A robust Process Control Block (PCB) system managing multiple process states (Ready, Running, Waiting, Blocked, Terminated).

### Memory Management Unit (MMU)
- **Virtual Memory:** Simulation of virtual to physical address translation.
- **Multi-Level Paging:** A two-level page table hierarchy for efficient memory management.
- **Page Replacement Algorithms:** Implements FIFO, LRU, and Clock policies.
- **Memory Protection:** Enforces Read, Write, and Execute (R/W/X) permissions on memory pages, simulating protection faults.

### CPU Scheduler
- **Scheduling Algorithms:** Implements Round Robin, non-preemptive Priority, and non-preemptive Shortest Job First (SJF).
- **I/O Blocking:** Realistically simulates processes moving between ready and waiting queues to handle I/O operations, improving CPU utilization.
- **Concurrency Simulation:** Features a functional Mutex to manage race conditions on a simulated shared resource.

### Basic File System
- **Inode-Based:** Simulates a simple file system using inodes, data blocks, and a free-block bitmap.
- **Core Operations:** Supports `create`, `write`, `read`, and `remove` file operations.

### Introspection & Visualization
- **System-Wide Stats:** A `stats` command to view live metrics on process states, page faults, and more.
- **ASCII Visualizations:** Graphical console printouts for the physical memory layout (`memmap`) and scheduler queues (`queues`).
- **Scalable Logging:** A multi-level logging system (Normal, Verbose, Debug) for deep-diving into the simulator's internal state.

---

## 🛠️ Getting Started

### Prerequisites
- A C++ compiler that supports the C++17 standard (e.g., g++).
- `make` build automation tool.

### Build Instructions

1.  **Clone the repository:**
    ```bash
    git clone <your-repo-url>
    cd mini-os-kernel-simulator
    ```

2.  **Compile the main simulator:**
    ```bash
    make
    ```
    This will create the main executable at `build/main`.

3.  **Compile the tests:**
    ```bash
    make test_scheduler
    make test_vm
    ```

### Running the Simulator

-   To run the main interactive CLI:
    ```bash
    make run
    ```
    or
    ```bash
    ./build/main
    ```

---

## 📖 Usage

The main simulator provides an interactive shell. Type `help` to see a full list of commands.

| Command                                     | Description                                                    |
| ------------------------------------------- | -------------------------------------------------------------- |
| `create <burst> <prio> [io] [io_freq]`      | Creates a new process.                                         |
| `run [steps]`                               | Runs the CPU scheduler, optionally for a set number of steps.  |
| `access <pid> <vpn> <type>`                 | Simulates a memory access (type: READ, WRITE, EXECUTE).        |
| `ps`                                        | Displays the list of all processes and their current state.    |
| `lock <pid>` / `unlock <pid>`               | Simulates a process acquiring or releasing a mutex.            |
| `mem <pid>`                                 | Shows the two-level page table for a specific process.         |
| `memmap`                                    | Displays a visual map of physical memory.                      |
| `queues`                                    | Displays a visual map of the scheduler's ready/waiting queues. |
| `stats`                                     | Shows current system-wide statistics.                          |
| `loglevel <0|1|2>`                          | Sets the system's verbosity (0=Normal, 1=Verbose, 2=Debug).    |
| `exit`                                      | Exits the simulator.                                           |
