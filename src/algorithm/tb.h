/*
 * tb.h
 *
 *  Created on: 17.04.2017
 *      Author: Klaus Steiner
 */

#ifndef ALGORITHM_TB_H_
#define ALGORITHM_TB_H_

#include <array>

#include "cell.hh"
#include "Eigen/Eigenvalues"
#include "Eigen/Dense"
#include "parser/hr/tbh.h"
#include "quan/quantityhandler.h"
#include "quan/rgrid.h"

namespace ks {

template<size_t Dim> class TightBindingAlgorithm : public CellAlgorithm<Dim,
        Eigen::MatrixXcd, Eigen::Vector3d> {
private:
   static_assert(Dim > 0, "Zero dimension is not supported");

   typedef ks_hr::TightBindingHamiltonian::NeighborType NeighborType;

   template<size_t D, size_t J> struct InitNeighborArray {
      static void init(std::array<int, D> &array,
                       const NeighborType &neighbor) {
         array[J] = std::get<J>(neighbor);
         InitNeighborArray<D, J + 1>::init(array, neighbor);
      }
   };

   template<size_t D> struct InitNeighborArray<D, Dim - 1> {
      static constexpr size_t J = Dim - 1;

      static void init(std::array<int, D> &array,
                       const NeighborType &neighbor) {
         array[J] = std::get<J>(neighbor);
      }
   };

public:
   TightBindingAlgorithm();

   void operator()(Eigen::MatrixXcd &H, const Eigen::Vector3d &k,
                   const std::array<size_t, Dim> &cell,
                   const std::array<size_t, Dim> &grid_size) override;

private:
   ks_hr::TightBindingHamiltonian *tb;
   size_t num_atoms;
   std::array<BoundaryCondition, Dim> bc_i;

};

}

#endif /* ALGORITHM_TB_H_ */
