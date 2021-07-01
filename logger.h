#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <chrono>
#include <ctime>

class Logger {
public:
    static Logger* instance() {
        if(internal_inst == nullptr) { internal_inst = new Logger; }
        return internal_inst;
    }

    template<typename T>
    void write(const T& t) {
        log << t;
    }

    template<typename T>
    void writeLine(const T& t) {
        log << t << std::endl;
    }

    void writeTime() {
        time_t res = time(nullptr);
        struct tm * crnt_time;
        char buff[26];
        crnt_time = localtime(&res);
        strftime(buff, sizeof buff, "%a %m/%d/%Y %H:%M:%S", crnt_time);
        log << buff << ": ";
        //auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        //log << std::ctime(&time) << ": ";
    }

    void close() {
        log.close();
    }

private:
    // Privatize the Default Constructor for Singleton
    Logger() { log = std::ofstream("output_log.txt"); };

    // Disabled for Singleton
    Logger(const Logger&) {}
    Logger& operator=(const Logger&) {}

    static Logger* internal_inst;
    std::ofstream log;
};

//Logger* Logger::internal_inst = nullptr;

#endif // LOGGER_H
