/*
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
