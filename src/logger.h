/**
 * This file is part of toplogy.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * logger.h
 *
 *  Created on: 11.06.2017
 *      Author: Klaus Steiner
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
