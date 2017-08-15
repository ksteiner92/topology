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
 * quantity.h
 *
 *  Created on: 26.06.2017
 *      Author: Klaus Steiner
 */

#ifndef QUANTITY_H_
#define QUANTITY_H_

#include <string>

#include "parser/config/config.h"

namespace ks {

class QuantityBase {
   friend class QuantityHandler;

public:
   virtual bool init(ks_conf::Config *config) = 0;

   const std::string getName() const {
      return name;
   }

protected:
   std::string name;
};

template<class T> class Quantity : public QuantityBase {
public:
   virtual T get() const {
      return value;
   }

protected:
   T value;
};

template<class T> class NumberQuantity : public Quantity<T> {
public:
   NumberQuantity() : NumberQuantity(0) {
      havedefval = false;
   }

   NumberQuantity(const T &defval) : defval(defval), havedefval(true) {
   }

   ~NumberQuantity() {}

   static QuantityBase *getQuantity(const T &defval) {
      return new NumberQuantity<T>(defval);
   }

   virtual bool init(ks_conf::Config *config) override {
      if (havedefval) {
         if (!config->get<T>(QuantityBase::getName(), Quantity<T>::value))
            Quantity<T>::value = defval;
      } else
         Quantity<T>::value = config->get<T>(QuantityBase::getName());
      return true;
   }

private:
   std::string node;
   T defval;
   bool havedefval;
};

}

#endif /* QUANTITY_H_ */
