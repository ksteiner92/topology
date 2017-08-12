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
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <type_traits>
#include <functional>
#include <utility>

#include "confparser.hh"
#include "confscanner.h"
#include "confobj.h"
#include "Eigen/Eigenvalues"
#include "Eigen/Dense"
#include "types.h"

namespace ks_conf {

class ConfigException : public std::exception {
public:
   explicit ConfigException(std::string what) :
           what(std::move(what)) {
   }

   const std::string &getWhat() const {
      return what;
   }

private:
   std::string what;
};

template<class In, class Out> struct NumberCast {
   static Out cast(const In &number) {
      return number;
   }
};

template<class T> struct NumberCast<std::complex<double>, T> {
   static T cast(const std::complex<double> &number) {
      return number.real();
   }
};

template<> struct NumberCast<std::complex<double>, std::complex<double>> {
   static std::complex<double> cast(const std::complex<double> &number) {
      return number;
   }
};

class Config {
   friend class ConfigParser;

   friend class ConfigScanner;

public:
   Config();

   virtual ~Config();

   bool parse(std::istream &in);

   bool parse(const std::string &file_name);

   std::map<std::string, ConfObject *> &getTree();

   template<class T> const bool get(const std::string &name, T &val) const {
      std::map<std::string, ConfObject *> level_tree = tree;
      return parseTreeString(name,
                             [&val](ConfObject *obj) {
                                typedef typename ConfObjectCastHelper<T>::type ConfObjectType;
                                val = NumberCast<typename std::result_of<decltype(&ConfObjectType::getValue)(
                                        ConfObjectType)>::type, T
                                >::cast(static_cast<ConfObjectType *>(obj)->getValue());
                                return true;
                             }, level_tree);
   }

   template<class T> const T get(const std::string &name) const {
      T res;
      std::map<std::string, ConfObject *> level_tree = tree;
      if (parseTreeString(name,
                          [&res](ConfObject *obj) {
                             typedef typename ConfObjectCastHelper<T>::type ConfObjectType;
                             res = NumberCast<typename std::result_of<decltype(&ConfObjectType::getValue)(
                                     ConfObjectType)>::type, T
                             >::cast(static_cast<ConfObjectType *>(obj)->getValue());
                             return true;
                          }, level_tree))
         return res;
      std::stringstream ss;
      ss << "Could not find value for " << name;
      throw ConfigException(ss.str());
   }

   template<class T> const T get(const std::string &name,
                                 const T &def_val) const {
      T res;
      std::map<std::string, ConfObject *> level_tree = tree;
      if (parseTreeString(name,
                          [&res](ConfObject *obj) {
                             typedef typename ConfObjectCastHelper<T>::type ConfObjectType;
                             res = NumberCast<typename std::result_of<decltype(&ConfObjectType::getValue)(
                                     ConfObjectType)>::type, T
                             >::cast(static_cast<ConfObjectType *>(obj)->getValue());
                             return true;
                          }, level_tree))
         return res;
      return def_val;
   }

   const std::map<std::string, ConfObject *> get(
           const std::string &name) const {
      std::map<std::string, ConfObject *> level_tree = tree;
      std::stringstream ss;
      ss.str(name);
      std::string node;
      bool found = false;
      while (std::getline(ss, node, '.')) {
         ConfObject *obj = level_tree[node];
         if (obj == nullptr) {
            found = false;
            break;
         }
         if (obj->getType() == OBJECT) {
            found = true;
            level_tree = dynamic_cast<Object *>(obj)->getValue();
         } else {
            found = false;
            break;
         }
      }
      if (!found) {
         std::map<std::string, ConfObject *> empty;
         return empty;
      }
      return level_tree;
   }

   ConfObject *getNode(const std::string &name) const {
      ConfObject *obj = getNodeOrNull(name);
      if (obj == nullptr) {
         std::stringstream ss;
         ss << "Could not find node " << name;
         throw ConfigException(ss.str());
      } else
         return obj;
   }

   ConfObject *getNodeOrNull(const std::string &name) const {
      std::map<std::string, ConfObject *> level_tree = tree;
      std::stringstream ss;
      ss.str(name);
      std::string node;
      ConfObject *obj = nullptr;
      while (std::getline(ss, node, '.')) {
         obj = level_tree[node];
         if (obj == nullptr)
            return nullptr;
         if (obj->getType() == OBJECT)
            level_tree = dynamic_cast<Object *>(obj)->getValue();
      }
      return obj;
   }

   const bool isObject(const std::string &name) const {
      std::map<std::string, ConfObject *> level_tree = tree;
      std::stringstream ss;
      ss.str(name);
      std::string node;
      bool object = false;
      while (std::getline(ss, node, '.')) {
         ConfObject *obj = level_tree[node];
         if (obj == nullptr) {
            std::stringstream ss;
            ss << "Could not find node " << name;
            throw ConfigException(ss.str());
         }
         if (obj->getType() == OBJECT) {
            object = true;
            level_tree = dynamic_cast<Object *>(obj)->getValue();
         } else
            object = false;
      }
      return object;
   }

   const bool exists(const std::string &name) const {
      std::map<std::string, ConfObject *> level_tree = tree;
      std::stringstream ss;
      ss.str(name);
      std::string node;
      while (std::getline(ss, node, '.')) {
         ConfObject *obj = level_tree[node];
         if (obj == nullptr)
            return false;
         if (obj->getType() == OBJECT)
            level_tree = dynamic_cast<Object *>(obj)->getValue();
      }
      return true;
   }

private:
   template<class T> struct ConfObjectCastHelper {
      typedef typename ks::IfElse<
              std::is_integral<T>::value || std::is_floating_point<T>::value,
              NumberObject,
              typename ks::IfElse<std::is_convertible<T, std::string>::value,
                      StringObject,
                      typename ks::IfElse<
                              std::is_convertible<T, std::complex<double>>::value,
                              NumberObject, void>::type>::type>::type type;
   };

   void switchInputStream(std::istream *is);

   void increaseLocation(unsigned int loc);

   const size_t getLocation() const;

   void insert(const std::pair<std::string, ConfObject *> &item);

   void deleteConfObject(ConfObject *obj);

   const bool parseTreeString(const std::string &tree_str,
                              const std::function<bool(ConfObject *)> &lambda,
                              std::map<std::string, ConfObject *> &tree, bool ignore_lambda_ret =
   false) const {
      std::stringstream ss;
      ss.str(tree_str);
      std::string node;
      bool found = ignore_lambda_ret;
      while (std::getline(ss, node, '.')) {
         if (ignore_lambda_ret && found)
            return false;
         ConfObject *obj = tree[node];
         if (obj == nullptr)
            return false;
         if (obj->getType() == OBJECT) {
            tree = dynamic_cast<Object *>(obj)->getValue();
         } else {
            found = lambda(obj);
            if (!found)
               return false;
         }
      }
      return found;
   }

private:
   ConfigScanner scanner;
   ConfigParser parser;
   size_t location;
   std::map<std::string, ConfObject *> tree;
};

}

#endif // CONFIG_H
