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
 * logger.cc
 *
 *  Created on: 11.06.2017
 *      Author: Klaus Steiner
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
