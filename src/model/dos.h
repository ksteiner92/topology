/*
 * dosmodel.h
 *
 *  Created on: 20.04.2017
 *      Author: klaus
 */

#ifndef MODEL_DOS_H_
#define MODEL_DOS_H_

#include <array>
#include <map>

#include "modelfactory.h"
#include "model.hh"
#include "algorithm/tb.h"
#include "algorithm/grid.hh"
#include "parser/sw/selfenergy.h"
#include "parser/hr/tbh.h"
#include "model/bs.h"
#include "quan/kpathbase.h"

namespace ks {

template<size_t Dim> class DensityOfStatesModel : public Model {
MODEL(DensityOfStatesModel)
public:
   DensityOfStatesModel();

   ~DensityOfStatesModel();

   void operator()(std::ostream &output) override;

private:
   GridAlgorithm<Dim, Eigen::MatrixXcd, Eigen::Vector3d> *grid_alg;
   TightBindingAlgorithm<Dim> *tb_alg;
   KPathBase *kpath;
   SelfEnergyBase *sw;
   size_t size;
};

} /* namespace ks */

#endif /* MODEL_DOS_H_ */
