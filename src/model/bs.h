/*
 * bs.h
 *
 *  Created on: 18.04.2017
 *      Author: Klaus Steiner
 */

#ifndef MODEL_BS_H_
#define MODEL_BS_H_

#include <array>
//#ifdef _OPENMP
#include <omp.h>
//#endif

#include "model.hh"
#include "algorithm/tb.h"
#include "algorithm/grid.hh"
#include "algorithm/sw.h"
#include "Eigen/Eigenvalues"
#include "Eigen/Dense"
#include "modelfactory.h"
#include "model.hh"
#include "parser/hr/tbh.h"
#include "quan/kpathbase.h"
#include "quan/rgrid.h"

namespace ks {

template<size_t Dim> class BandStructureModel : public Model {
MODEL(BandStructureModel)
public:
   BandStructureModel();

   virtual ~BandStructureModel();

   void operator()(std::ostream &output) override;

protected:
   TightBindingAlgorithm<Dim> *tb_alg;
   size_t size;
   GridAlgorithm<Dim, Eigen::MatrixXcd, Eigen::Vector3d> *grid_alg;
   KPathBase *kpath;
};

template<size_t Dim> class RealSwBandStructureModel : public BandStructureModel<Dim> {
MODEL(RealSwBandStructureModel)
   typedef BandStructureModel<Dim> super;
public:
   RealSwBandStructureModel();

   ~RealSwBandStructureModel() override;

   void operator()(std::ostream &output) override;

private:
   GridAlgorithm<Dim, Eigen::MatrixXcd, size_t> *grid_sw_alg;
   SelfEnergyAlgorithm<Dim> *sw_alg;
};

}

#endif /* MODEL_BS_H_ */
