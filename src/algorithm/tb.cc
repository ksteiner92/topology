/*
 * tb.cc
 *
 *  Created on: 20.04.2017
 *      Author: Klaus Steiner
 */

#include "tb.h"

using namespace std;
using namespace Eigen;
using namespace ks_hr;

namespace ks {

template<size_t Dim> TightBindingAlgorithm<Dim>::TightBindingAlgorithm() {
   QuantityHandler *quanHandler = QuantityHandler::get();
   tb = quanHandler->getValue<TightBindingHamiltonian *>("hr");
   auto *rgrid = quanHandler->getValue<RealGrid *>("rgrid");
   bc_i = rgrid->getBoundaryConditions<Dim>();
   const array<size_t, Dim> grid_size = rgrid->getGridSize<Dim>();
   num_atoms = tb->getNumAtoms();
}

template<size_t Dim> void TightBindingAlgorithm<Dim>::operator()(MatrixXcd &H,
                                                                 const Vector3d &k, const array<size_t, Dim> &cell,
                                                                 const array<size_t, Dim> &grid_size) {
   const size_t cell_idx = getUniquePosIdx<Dim>(cell, grid_size);
   for (const auto &it : tb->getTij()) {
      const MatrixXcd t_ij = it.second;
      array<int, Dim> neighbor;
      InitNeighborArray<Dim, 0>::init(neighbor, it.first);
      bool include_cell = true;
      array<size_t, Dim> neighbor_cell;
      for (size_t i = 0; i < Dim; i++)
         neighbor_cell[i] = 0;
      for (size_t i = 0; i < Dim && include_cell; i++) {
         switch (bc_i[i]) {
            case OPEN: {
               const long int tmp = cell[i] + neighbor[i];
               if (tmp < 0 || tmp > (grid_size[i] - 1))
                  include_cell = false;
               else
                  neighbor_cell[i] = tmp;
               break;
            }
            case PERIODIC:
               const long int mod = neighbor[i] % grid_size[i];
               const long int tmp = cell[i]
                                    + ((neighbor[i] < 0 ? -1 : 1) * mod);
               if (tmp < 0)
                  neighbor_cell[i] = tmp + grid_size[i];
               else if (tmp >= grid_size[i])
                  neighbor_cell[i] = tmp - grid_size[i];
               break;
         }
      }
      if (!include_cell)
         continue;
      const size_t neighbor_idx = getUniquePosIdx<Dim>(neighbor_cell,
                                                       grid_size);
      const Vector3d r = tb->getPosOfNeighbor(it.first);
      const complex<double> phase = exp(-1i * r.dot(k));
      for (size_t atom1 = 0; atom1 < num_atoms; atom1++) {
         for (size_t atom2 = 0; atom2 < num_atoms; atom2++) {
            H(cell_idx * num_atoms + atom1,
              neighbor_idx * num_atoms + atom2) += t_ij(atom1, atom2)
                                                   * phase;
         }
      }
   }
}

template class TightBindingAlgorithm<1>;

template class TightBindingAlgorithm<2>;

template class TightBindingAlgorithm<3>;

}

