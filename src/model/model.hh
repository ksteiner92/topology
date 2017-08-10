/*
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
