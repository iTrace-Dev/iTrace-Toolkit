#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <chrono>
#include <ctime>
#include <cstdarg>

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
        time_t t = time(nullptr);
        char buff[63];
        struct tm crnt_time;
        //localtime_s(&crnt_time,&t);
        strftime(buff, sizeof buff, "%H:%M:%S :: ", &crnt_time); //"%a %m/%d/%Y %H:%M:%S :: "

        log << buff;
    }

    // Disabled for Singleton
    Logger(const Logger&) {}
    Logger& operator=(const Logger&) {}

    static Logger* internal_inst;
    std::ofstream log;

    bool lineEnded = false;
};

#endif // LOGGER_H
