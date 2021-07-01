#ifndef LOGGER_H
#define LOGGER_H


class Logger
{
public:
    static Logger* instance();

private:
    Logger() {};

    Logger(const Logger&) {}
    Logger& operator=(const Logger&) {}

    static Logger* internal_inst;
};

Logger* Logger::internal_inst = nullptr;

#endif // LOGGER_H
