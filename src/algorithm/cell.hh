/*
 * cell.hh
 *
 *  Created on: 17.04.2017
 *      Author: Klaus Steiner
 */

#ifndef ALGORITHM_CELL_HH_
#define ALGORITHM_CELL_HH_

#include <array>

#include "algorithm.h"

namespace ks {

/**
 * Possible boundary conditions
 * which are supported.
 */
enum BoundaryCondition {
   OPEN, PERIODIC
};

/**
 * Returns for a given tensor position and size
 * an unique index.
 *
 * @tparam Dim Rank of the tensor
 * @param pos Tensor position tuple
 * @param dims Tensor size
 * @return a unique index for given input.
 */
template<size_t Dim> static constexpr size_t getUniquePosIdx(
        const std::array<size_t, Dim> &pos,
        const std::array<size_t, Dim> &dims) {
   size_t arrayPos = 0;
   for (size_t i = 1; i <= Dim; i++) {
      size_t offset = 1;
      for (size_t j = i + 1; j <= Dim; j++)
         offset *= dims[j - 1];
      arrayPos += offset * pos[i - 1];
   }
   return arrayPos;
}

/**
 * Base class for Cell algorithm. A cell algorithm is a algorithm
 * which is executed for every cell in a system. This is done by
 * @class GridAlgorithm which takes a cell algorithm as parameter.
 *
 * @tparam Dim Dimension of the System
 * @tparam Output type the algorithm
 * @tparam Input types of the algorithm
 */
template<size_t Dim, class Output, class ... Input> class CellAlgorithm : public Algorithm<
        Output, Input..., std::array<size_t, Dim>, std::array<size_t, Dim>> {
};

}

#endif /* ALGORITHM_CELL_HH_ */
