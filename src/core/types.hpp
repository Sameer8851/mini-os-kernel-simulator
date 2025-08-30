#ifndef TYPES_HPP
#define TYPES_HPP

enum LogLevel {
    NORMAL,
    VERBOSE,
    DEBUG
};

enum class AccessType {
    READ,
    WRITE,
    EXECUTE
};

enum class ProcessState {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
};

#endif