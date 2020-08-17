#ifndef LOGGER_H
#define LOGGER_H


#define PREFIX "LOG_"
#include <chrono>
#include <ctime>
#include <string>
#include <iostream>
#include <vector>

#include <fstream>
class Logger
{
public:
    Logger();
    void start_log();
    void save_log_to_file();

    void end_log();
    void update(std::string message);

private:
    std::vector<std::string> buffer;
};

#endif // LOGGER_H
