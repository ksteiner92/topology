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
   virtual ~BandStructureModel();

   virtual bool init(QuantityHandler* quanHandler) override;

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
   ~RealSwBandStructureModel() override;

   bool init(QuantityHandler* quanHandler) override;

   void operator()(std::ostream &output) override;

private:
   GridAlgorithm<Dim, Eigen::MatrixXcd, size_t> *grid_sw_alg;
   SelfEnergyAlgorithm<Dim> *sw_alg;
};

}

#endif /* MODEL_BS_H_ */
