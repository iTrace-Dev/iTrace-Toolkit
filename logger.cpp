#include "logger.h"

Logger::Logger() {
    log = std::ofstream("output_log.txt");
}

Logger* Logger::instance() {
    if(internal_inst == nullptr) { internal_inst = new Logger; }
    return internal_inst;
}

void Logger::write(const std::string& str) {
    log << str;
}

void Logger::writeLine(const std::string& str) {
    log << str << std::endl;
}

void Logger::write(int i) {
    log << i;
}

void Logger::writeLine(int i) {
    log << i << std::endl;
}

void Logger::close() {
    log.close();
}
