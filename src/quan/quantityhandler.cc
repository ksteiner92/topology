/*
 * quantityfactory.cc
 *
 *  Created on: 26.06.2017
 *      Author: klaus
 */

#include "quantityhandler.h"

using namespace std;
using namespace ks_conf;

namespace ks {

QuantityHandler::QuantityHandler() = default;

QuantityHandler::~QuantityHandler() {
   for (auto it : quans)
      delete it.second;
}

QuantityHandler *QuantityHandler::get() {
   if (singleton == nullptr)
      singleton = new QuantityHandler();
   return singleton;
}

bool QuantityHandler::registerQuantity(const std::string &name, QuantityBase *quan) {
   quan->name = name;
   quans.insert(make_pair(name, quan));
   initquans.insert(make_pair(name, false));
   return true;
}

void QuantityHandler::setConfig(ks_conf::Config *config) {
   this->config = config;
   for (auto it : initquans)
      it.second = false;
}

QuantityBase *QuantityHandler::getQuantity(const string &name, const bool mandatory) {
   QuantityBase *quan = quans[name];
   if (quan != nullptr && !initquans[name]) {
      const bool initstat = quan->init(config);
      if (!initstat && mandatory) {
         stringstream ss;
         ss << "Could not initialize " << name << " which is needed by a model.";
         throw ConfigException(ss.str());
      } else if (!initstat)
         return nullptr;
      initquans.insert(make_pair(name, initstat));
   } else if (quan == nullptr && mandatory) {
      stringstream ss;
      ss << "Could not initialize " << name << " which is needed by a model.";
      throw ConfigException(ss.str());
   }
   return quan;
}

QuantityHandler *QuantityHandler::singleton = nullptr;

} /* namespace ks */
