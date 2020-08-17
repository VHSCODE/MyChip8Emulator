#include "logger.h"

Logger::Logger()
{

}



void Logger::start_log()
{
    std::time_t start_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string msg =  "*** Program started at " + (std::string) std::ctime(&start_time) + " ***\n";

    buffer.push_back(msg);
    std::cout << msg << std::endl;
}



void Logger::update(std::string message)
{
    std::string msg = "[ " + message + " ]";

    buffer.push_back(msg);
    std::cout << msg << std::endl;
}



void Logger::end_log()
{
    std::time_t end_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::string msg =  "Program finished at " + (std::string) std::ctime(&end_time);

    buffer.push_back(msg);

    std::cout << msg << std::endl;
}



void Logger::save_log_to_file()
{
    std::time_t end_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::string name = PREFIX + (std::string) std::ctime(&end_time)+".txt";

    std::ofstream file;

    file.open(name);

    if(file){

        file.write((char*)&buffer[0],buffer.size());
    }

    file.close();
}
