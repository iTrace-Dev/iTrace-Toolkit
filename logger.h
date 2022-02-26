/********************************************************************************************************************************************************
* @file logger.h
*
* @Copyright (C) 2022 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/

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
