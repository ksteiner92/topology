/*
 * logger.h
 *
 *  Created on: 11.06.2017
 *      Author: klaus
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>
#include <string>
#include <sstream>

namespace ks {

enum LogLevel {
   DEBUG, INFO, WARNING, ERROR
};

class Logger {
private:
   static Logger *singleton;

   Logger();

public:
   static Logger *get();

   void log(const std::string &message);

   Logger &operator<<(const std::string &message);

   Logger &operator<<(const char *message);

   Logger &operator<<(LogLevel level);

private:
   LogLevel level;
   bool logtime;
};

#define LOG (*Logger::get())

#define LOG_INFO LOG << LogLevel::INFO

#define LOG_WITH_TIME(MSG)\
   Logger::get() << __TIME__ << ": " << MSG;

} /* namespace ks */

#endif /* LOGGER_H_ */
