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
