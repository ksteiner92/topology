/*
 * modelfactory.cc
 *
 *  Created on: 05.06.2017
 *      Author: Klaus Steiner
 */

#include "modelfactory.h"

using namespace std;

namespace ks {

ModelFactory::ModelFactory() = default;

ModelFactory::~ModelFactory() = default;

ModelFactory *ModelFactory::get() {
   if (singleton == nullptr)
      singleton = new ModelFactory();
   return singleton;
}

Model *ModelFactory::create(const string &name) {
   auto registeredPair =
           ModelFactory::reg.find(name);
   if (registeredPair == ModelFactory::reg.end())
      return nullptr;
   Model *model = registeredPair->second();
   model->name = name;
   return model;
}

bool ModelFactory::registerModel(const std::string &name,
                                 Creator createMethod) {
   std::pair<std::map<std::string, Creator>::iterator, bool> registeredPair =
           ModelFactory::reg.insert(
                   std::make_pair(name.c_str(), createMethod));
   return registeredPair.second;
}

ModelFactory *ModelFactory::singleton = nullptr;

} /* namespace ks */
