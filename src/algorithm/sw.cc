/*
 * sw.cc
 *
 *  Created on: 20.04.2017
 *      Author: Klaus Steiner
 */

#include "sw.h"

using namespace std;
using namespace Eigen;
using namespace ks_hr;

namespace ks {

template<size_t Dim> SelfEnergyAlgorithm<Dim>::SelfEnergyAlgorithm() {
   QuantityHandler *quanHandler = QuantityHandler::get();
   sw_edge = quanHandler->getValue<SelfEnergyBase *>("sw_edge", false);
   sw = quanHandler->getValue<SelfEnergyBase *>("sw", false);
   num_atoms = quanHandler->getValue<TightBindingHamiltonian *>("hr")->getNumAtoms();
   bc = quanHandler->getValue<RealGrid *>("rgrid")->getBoundaryConditions<Dim>();
   sw_zero.resize(num_atoms, 0.0);
}

template<size_t Dim> void SelfEnergyAlgorithm<Dim>::operator()(MatrixXcd &sigma,
                                                               const size_t &widx, const array<size_t, Dim> &cell,
                                                               const array<size_t, Dim> &grid_size) {
   const size_t idx = getUniquePosIdx<Dim>(cell, grid_size);
   bool on_edge = false;
   for (size_t i = 0; i < Dim; i++) {
      if (bc[i] == OPEN
          && (cell[i] == 0 || cell[i] == (grid_size[i] - 1) /*|| cell[i] == 1 || cell[i] == (grid_size[i] - 2) */)) {
         on_edge = true;
         break;
      }
   }
   vector<complex<double>> sw_omega;
   sw_omega = on_edge && sw_edge != nullptr ? (*sw_edge)[widx] : (sw != nullptr ? (*sw)[widx] : sw_zero);
   for (size_t i = 0; i < num_atoms; i++) {
      const size_t n = idx * num_atoms + i;
      sigma(n, n) = sw_omega[i];
   }
}

template class SelfEnergyAlgorithm<1>;

template class SelfEnergyAlgorithm<2>;

template class SelfEnergyAlgorithm<3>;

}

