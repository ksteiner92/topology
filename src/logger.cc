/*
 * logger.cc
 *
 *  Created on: 11.06.2017
 *      Author: klaus
 */

#include "logger.h"

using namespace std;

namespace ks {

Logger::Logger() : level(INFO), logtime(false) {
}

Logger *Logger::get() {
   if (singleton == nullptr)
      singleton = new Logger();
   return singleton;
}

void Logger::log(const string &message) {
   stringstream ss;
   ss << '[';
   switch (level) {
      case DEBUG:
         ss << "Debug]: ";
         break;
      case INFO:
         ss << "Info]: ";
         break;
      case WARNING:
         ss << "Warning]: ";
         break;
      case ERROR:
         ss << "Error]: ";
         break;
   }
}

Logger &Logger::operator<<(LogLevel level) {
   this->level = level;
   return *this;
}

Logger &Logger::operator<<(const string &message) {
   return *this;
}

Logger &Logger::operator<<(const char *message) {
   return *this;
}

Logger *Logger::singleton = nullptr;


} /* namespace ks */
