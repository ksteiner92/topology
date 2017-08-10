/*
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
   virtual ~QuantityBase() {
   }

   virtual bool init(ks_conf::Config *config) = 0;

   const std::string getName() const {
      return name;
   }

protected:
   std::string name;
};

template<class T> class Quantity : public QuantityBase {
   struct Deleter {

   };
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
