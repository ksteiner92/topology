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
 * sw.h
 *
 *  Created on: 18.04.2017
 *      Author: Klaus Steiner
 */

#ifndef ALGORITHM_SW_H_
#define ALGORITHM_SW_H_

#include <array>
#include <complex>
#include <vector>

#include "cell.hh"
#include "quan/selfenergybase.h"
#include "quan/quantityhandler.h"
#include "quan/rgrid.h"
#include "parser/hr/tbh.h"
#include "Eigen/Eigenvalues"
#include "Eigen/Dense"

namespace ks {

template<size_t Dim> class SelfEnergyAlgorithm : public CellAlgorithm<Dim,
        Eigen::MatrixXcd, size_t> {
public:
   SelfEnergyAlgorithm();

   void operator()(Eigen::MatrixXcd &sw, const size_t &widx,
                   const std::array<size_t, Dim> &cell,
                   const std::array<size_t, Dim> &grid_size) override;

private:
   size_t num_atoms;
   std::vector<std::complex<double>> sw_zero;
   std::array<BoundaryCondition, Dim> bc;
   SelfEnergyBase *sw;
   SelfEnergyBase *sw_edge;

};

}

#endif /* ALGORITHM_SW_H_ */
