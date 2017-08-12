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
 * model.hh
 *
 *  Created on: 17.04.2017
 *      Author: Klaus Steiner
 */

#ifndef MODEL_MODEL_HH_
#define MODEL_MODEL_HH_

#include <iostream>

namespace ks {

/**
 * Model base class
 *
 * Base class model which every model from
 * which every model inherits.
 */
class Model {
   friend class ModelFactory;

public:
   /**
    * The name of the model
    *
    * @return the name of the model
    */
   const std::string getName() const {
      return name;
   }

   /**
    * Pure virtual operator. In this function each model
    * implements it's magic.
    *
    * @param stream the output stream which should be used
    * for displaying the output during calculations.
    */
   virtual void operator()(std::ostream &stream) = 0;

private:
   std::string name;
};

}

#endif /* MODEL_MODEL_HH_ */
