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
 * topinv.hh
 *
 *  Created on: 26.04.2017
 *      Author: Klaus Steiner
 */

#ifndef MODEL_TOPINV_HH_
#define MODEL_TOPINV_HH_

#include <array>
#include <complex>
#include <vector>

#include "model.hh"
#include "algorithm/tb.h"
#include "algorithm/grid.hh"
#include "parser/kpath/kpath.h"
#include "parser/hr/tbh.h"

namespace ks {

template<size_t Dim> class TopologicalInvariantBaseModel : public Model {
public:
   virtual bool init(QuantityHandler* quanHandler) override {
      tb_alg = new TightBindingAlgorithm<Dim>();
      grid_alg = new GridAlgorithm<Dim, Eigen::MatrixXcd, Eigen::Vector3d>(tb_alg);
      num_atoms = quanHandler->getValue<ks_hr::TightBindingHamiltonian *>("hr")->getNumAtoms();
      size = getGridVolume<Dim>(quanHandler->getValue<RealGrid *>("rgrid")->getGridSize<Dim>())
             * num_atoms;
      kdens = quanHandler->getValue<size_t>("kdens");
      if (kdens % 2 == 0)
         throw "Size of k-grid must be odd to include time reverse invariant points";
      bz = quanHandler->getValue<ks_hr::TightBindingHamiltonian *>("hr")->getBZ();
      return true;
   }

   virtual ~TopologicalInvariantBaseModel() {
      delete tb_alg;
      delete grid_alg;
   }

protected:
   TightBindingAlgorithm<Dim> *tb_alg;
   size_t size;
   size_t num_atoms;
   GridAlgorithm<Dim, Eigen::MatrixXcd, Eigen::Vector3d> *grid_alg;
   size_t kdens;
   Eigen::Matrix3d bz;

   inline const double getKGridPoint(const double k_min, const double k_max,
                                     const double dk, const size_t idx, const size_t size) const {
      double k = 0.0;
      if (idx == 0)
         k = k_min;
      else if (idx == (size - 1))
         k = k_max;
      else if (idx == (size / 2))
         k = 0.0;
      else
         k = k_min + idx * dk;
      return k;
   }

};

} /* namespace ks */

#endif /* MODEL_TOPINV_HH_ */
