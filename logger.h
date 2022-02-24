#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <chrono>
#include <ctime>
#include <cstdarg>

#include <QDateTime>

#include <iostream>

class Logger {
public:
    static Logger* instance() {
        if(internal_inst == nullptr) { internal_inst = new Logger; }
        return internal_inst;
    }

    /*template<typename T>
    void write(const T& t) {
        if(lineEnded) { writeTime(); }
        log << t;
        lineEnded = false;
    }*/

    template<typename T>
    void writeLine(const char* type, const T& text) {
        if(lineEnded) { writeTime(); }
        log << "[" << type << "] " << text << std::endl;
        lineEnded = true;
    }

    void close() {
        log.close();
    }

private:
    // Privatize the Default Constructor for Singleton
    Logger() { log = std::ofstream("output_log.txt"); writeTime(); writeLine("INFO","Log file created"); };

    // Function that writes time at the beginning of each log entry
    void writeTime() {
        log << QDateTime::currentDateTime().toString().toUtf8().constData() << ' ';
    }

    // Disabled for Singleton
    Logger(const Logger&) {}
    Logger& operator=(const Logger&) {}

    static Logger* internal_inst;
    std::ofstream log;

    bool lineEnded = false;
};

#endif // LOGGER_H
