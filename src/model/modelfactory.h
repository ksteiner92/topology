/*
 * modelfactory.h
 *
 *  Created on: 05.06.2017
 *      Author: Klaus Steiner
 */

#ifndef MODELFACTORY_H_
#define MODELFACTORY_H_

#include <string>
#include <map>

#include "model.hh"

namespace ks {

typedef Model *(*Creator)();

class ModelFactory {
private:
   ModelFactory();

   virtual ~ModelFactory();

public:
   static ModelFactory *get();

   bool registerModel(const std::string &name, Creator createMethod);

   Model *create(const std::string &name);

private:
   static ModelFactory *singleton;

   std::map<std::string, Creator> reg;
};

#define MODEL(ModelType)\
private:\
    static Model *create() {\
         return new ModelType();\
     }\
protected:\
    static const bool registered;

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define REGISTER_MODEL_ALL_DIMS(ModelType, ModelName)\
    template class ModelType<1>; \
    template<> const bool ModelType<1>::registered = ModelFactory::get()->registerModel( \
            #ModelName STR(_1), &ModelType<1>::create); \
    template class ModelType<2>; \
    template<> const bool ModelType<2>::registered = ModelFactory::get()->registerModel( \
            #ModelName STR(_2), &ModelType<2>::create);\
    template class ModelType<3>; \
    template<> const bool ModelType<3>::registered = ModelFactory::get()->registerModel( \
            #ModelName STR(_3), &ModelType<3>::create);

} /* namespace ks */

#endif /* MODELFACTORY_H_ */
