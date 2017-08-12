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
 * quantityfactory.h
 *
 *  Created on: 26.06.2017
 *      Author: Klaus Steiner
 */

#ifndef QUAN_QUANTITYFACTORY_H_
#define QUAN_QUANTITYFACTORY_H_

#include <map>
#include <vector>

#include "quantity.h"
#include "parser/config/config.h"

namespace ks {

class QuantityHandler {
private:
   static QuantityHandler *singleton;

   QuantityHandler();

   QuantityHandler(const QuantityHandler &) = default;

   virtual ~QuantityHandler();

public:
   static QuantityHandler *get();

   bool registerQuantity(const std::string &name, QuantityBase *quan);

   void setConfig(ks_conf::Config *config);

   QuantityBase *getQuantity(const std::string &name, bool mandatory);

   template<class T> T getValue(const std::string &name, const bool mandatory = true) {
      QuantityBase *quan = getQuantity(name, mandatory);
      if (quan != nullptr)
         return ((Quantity<T> *) quan)->get();
      if (!mandatory && std::is_pointer<T>::value)
         return NULL;
      std::stringstream ss;
      ss << "Could not get value of " << name << " not configured and not mandatory but no default value exists.";
      throw ks_conf::ConfigException(ss.str());
   }


protected:
   ks_conf::Config *config;
   std::map<std::string, QuantityBase *> quans;
   std::map<std::string, bool> initquans;
};

#define REGISTER_QUAN(QuanType, QuanName)\
    const bool reg_##QuanName = QuantityHandler::get()->registerQuantity( \
            #QuanName, new QuanType());

#define REGISTER_TEMPLATE_QUAN(QuanType, T, QuanName)\
    const bool reg_##QuanName = QuantityHandler::get()->registerQuantity( \
            #QuanName, new QuanType<T>());

#define REGISTER_NUMBER_QUAN(T, QuanName)\
    REGISTER_TEMPLATE_QUAN(NumberQuantity, T, QuanName);

#define REGISTER_NUMBER_QUAN_WITH_DEFAULT(T, QuanName, defval)\
    const bool reg_##QuanName = QuantityHandler::get()->registerQuantity( \
            #QuanName, new NumberQuantity<T>(defval));

} /* namespace ks */

#endif /* QUAN_QUANTITYFACTORY_H_ */
