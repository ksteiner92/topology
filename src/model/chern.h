/*
 * chernmodel.h
 *
 *  Created on: 23.04.2017
 *      Author: klaus
 */

#ifndef MODEL_CHERN_H_
#define MODEL_CHERN_H_

#include <array>
#include <complex>
#include <vector>

#include "modelfactory.h"
#include "model.hh"
#include "topinv.hh"

namespace ks {

template<size_t Dim> class ChernModel : public TopologicalInvariantBaseModel<Dim> {
   typedef TopologicalInvariantBaseModel<Dim> super;
MODEL(ChernModel)
public:
   ChernModel();

   void operator()(std::ostream &output) override;
};

} /* namespace ks */

#endif /* MODEL_CHERN_H_ */
