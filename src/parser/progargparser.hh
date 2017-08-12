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
 * progargparser.hh
 *
 *  Created on: 06.04.2017
 *      Author: Klaus Steiner
 */

#ifndef PROGARGPARSER_HH_
#define PROGARGPARSER_HH_

#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <set>
#include <functional>

#include "types.h"

namespace ks {

struct ProgArgTypes {
   enum ProgArgType {
      Int, Float, String, Flag
   };
};

template<ProgArgTypes::ProgArgType T> struct ArgType {
   typedef void type;

   static auto defaultParser(const int argc, char **argv, const int pos, const int multi, std::vector<type> &values) {
      return 0;
   };
};

template<> struct ArgType<ProgArgTypes::Flag> {
   typedef std::string type;

   static auto defaultParser(const int argc, char **argv, const int pos, const int multi, std::vector<type> &values) {
      return 0;
   };
};

template<> struct ArgType<ProgArgTypes::Int> {
   typedef int type;

   static auto defaultParser(const int argc, char **argv, const int pos,
                             const int multi, std::vector<type> &values) {
      size_t i = pos;
      const size_t max = (multi < 0 ? (argc - 1) : (pos + multi - 1));
      for (i = pos; i <= max; i++) {
         if (i >= argc)
            throw "Too less arguments given";
         std::string value_token(argv[i]);
         values.push_back(std::stoi(value_token));
      }
      return i - pos;
   }
};

template<> struct ArgType<ProgArgTypes::Float> {
   typedef double type;

   static auto defaultParser(const int argc, char **argv, const int pos, const int multi, std::vector<type> &values) {
      size_t i = pos;
      const size_t max = (multi < 0 ? (argc - 1) : (pos + multi - 1));
      for (i = pos; i <= max; i++) {
         if (i >= argc)
            throw "Too less arguments given";
         std::string value_token(argv[i]);
         values.push_back(std::stod(value_token));
      }
      return i - pos;
   }
};

template<> struct ArgType<ProgArgTypes::String> {
   typedef std::string type;

   static auto defaultParser(const int argc, char **argv, const int pos, const int multi, std::vector<type> &values) {
      size_t i = pos;
      const size_t max = (multi < 0 ? (argc - 1) : (pos + multi - 1));
      for (i = pos; i <= max; i++) {
         if (i >= argc)
            throw "Too less arguments given";
         std::string value_token(argv[i]);
         values.push_back(value_token);
      }
      return i - pos;
   }
};

template<ProgArgTypes::ProgArgType T> struct ParserFunctionType {
   typedef std::function<
           int(const int argc, char **argv, const int pos, const int multi,
               std::vector<typename ArgType<T>::type> &values)> type;
};

class IProgArg {
public:
   virtual ~IProgArg() {
   }

   virtual const int parse(const int argc, char **argv, const int pos) = 0;
};

class ProgArgParser;

template<ProgArgTypes::ProgArgType T> class ProgArg : public IProgArg {
   friend ProgArgParser;
private:
   ProgArg() :
           man(false), num_para(1), num_parsed_args(0), p(
           &ArgType<T>::defaultParser) {
   }

public:
   virtual ~ProgArg() {
   }

   const int parse(const int argc, char **argv, const int pos) {
      num_parsed_args = p(argc, argv, pos, num_para, values);
      return num_parsed_args;
   }

   const int getNumParsedArgs() const {
      return num_parsed_args;
   }

   const bool available() const {
      return true;
   }

   const typename ArgType<T>::type &get(const size_t idx) const {
      return values[idx];
   }

   const std::vector<typename ArgType<T>::type> &get() const {
      return values;
   }

   const typename ArgType<T>::type operator[](const size_t idx) const {
      return values[idx];
   }

   ProgArg &parser(const typename ParserFunctionType<T>::type &p) {
      this->p = p;
      return *this;
   }

   ProgArg &description(const std::string &desc) {
      this->desc = desc;
      return *this;
   }

   ProgArg &abbreviation(const std::string &abb) {
      abbreviations.insert(abb);
      return *this;
   }

   ProgArg &multi(const size_t num_para) {
      this->num_para = num_para;
      return *this;
   }

   ProgArg &mandatory(const bool man) {
      this->man = man;
      return *this;
   }

   const bool isMandatory() const {
      return man;
   }

   std::set<std::string> getAbbreviation() const {
      return abbreviations;
   }

   const std::string &getDescription() const {
      return desc;
   }

private:
   typedef size_t (*ParserFunction)(const int, char **, const int, const int, std::vector<typename ArgType<T>::type> &);

   ParserFunction p;
   std::string desc;
   std::set<std::string> abbreviations;
   std::vector<typename ArgType<T>::type> values;
   size_t num_para;
   bool man;
   int num_parsed_args;
};

class ProgArgParser {
public:
   ProgArgParser() {
      arg<ProgArgTypes::String>("").multi(-1);
   }

   virtual ~ProgArgParser() {
   }

   const bool operator()(int argc, char **argv) {
      std::map<std::string, std::string> assigned_abbreviations;
      std::set<std::string> mandatory_args;
      for (const auto &arg_pair : args) {
         const ProgArgTypes::ProgArgType type = types[arg_pair.first];
         assigned_abbreviations.insert(
                 std::pair<std::string, std::string>(arg_pair.first,
                                                     arg_pair.first));
         std::set<std::string> abbreviations;
         switch (type) {
            case ProgArgTypes::Int:
               abbreviations =
                       static_cast<ProgArg<ProgArgTypes::Int> *>(arg_pair.second.get())->getAbbreviation();
               if (static_cast<ProgArg<ProgArgTypes::Int> *>(arg_pair.second.get())->isMandatory())
                  mandatory_args.insert(arg_pair.first);
               break;
            case ProgArgTypes::Float:
               abbreviations =
                       static_cast<ProgArg<ProgArgTypes::Float> *>(arg_pair.second.get())->getAbbreviation();
               if (static_cast<ProgArg<ProgArgTypes::Float> *>(arg_pair.second.get())->isMandatory())
                  mandatory_args.insert(arg_pair.first);
               break;
            case ProgArgTypes::String:
               abbreviations =
                       static_cast<ProgArg<ProgArgTypes::String> *>(arg_pair.second.get())->getAbbreviation();
               if (static_cast<ProgArg<ProgArgTypes::String> *>(arg_pair.second.get())->isMandatory())
                  mandatory_args.insert(arg_pair.first);
               break;
            case ProgArgTypes::Flag:
               abbreviations =
                       static_cast<ProgArg<ProgArgTypes::Flag> *>(arg_pair.second.get())->getAbbreviation();
               if (static_cast<ProgArg<ProgArgTypes::Flag> *>(arg_pair.second.get())->isMandatory())
                  mandatory_args.insert(arg_pair.first);
               break;

         }
         for (const std::string &abbreviation : abbreviations)
            assigned_abbreviations.insert(
                    std::pair<std::string, std::string>(abbreviation,
                                                        arg_pair.first));
      }
      std::set<std::string> parsed_args;
      for (size_t i = 1; i < argc; i++) {
         const std::string token(argv[i]);
         const std::string designator = assigned_abbreviations[token];
         parsed_args.insert(designator);
         if (!designator.empty()) {
            const auto it = args.find(designator);
            if (it != args.end())
               i += it->second.get()->parse(argc, argv, i + 1);
         } else
            i += args.find("")->second.get()->parse(argc, argv, i);
      }
      for (const std::string &mandatory_arg : mandatory_args) {
         if (parsed_args.find(mandatory_arg) == parsed_args.end()) {
            std::cerr << "Argument '" << mandatory_arg << "' missing: "
                      << std::endl;
            return false;
         }
      }
      return true;
   }

   template<ProgArgTypes::ProgArgType T> ProgArg<T> &arg(
           const std::string &designator) {
      ProgArg<T> *arg = NULL;
      const auto it = args.find(designator);
      if (it == args.end()) {
         arg = new ProgArg<T>;
         args.insert(
                 std::pair<std::string, std::unique_ptr<IProgArg>>(
                         designator, std::unique_ptr<IProgArg>(arg)));
      } else
         arg = static_cast<ProgArg<T> *>(it->second.get());
      types[designator] = T;
      return *arg;
   }

private:
   std::map<std::string, ProgArgTypes::ProgArgType> types;
   std::map<std::string, std::unique_ptr<IProgArg>> args;
};

} /* namespace ks */

#endif /* PROGARGPARSER_HH_ */
