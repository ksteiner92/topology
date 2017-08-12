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
 * grid.hh
 *
 * Created on: 17.04.2017
 *     Author: Klaus Steiner
 */

#ifndef ALGORITHM_GRID_HH_
#define ALGORITHM_GRID_HH_

#include <array>
#include <functional>

#include "algorithm.h"
#include "cell.hh"
#include "quan/quantityhandler.h"

namespace ks {

/**
 * Calculates the grid volume.
 * @tparam Dim Dimension of the system.
 * @param grid_size The size of the grid.
 * @return The volume of the grid of dimension Dim.
 */
template<size_t Dim> static constexpr size_t getGridVolume(
        const std::array<size_t, Dim> &grid_size) {
   size_t vol = 1;
   for (size_t i = 0; i < Dim; i++)
      vol *= grid_size[i];
   return vol;
}

/**
 *
 * @tparam Dim System dimension
 * @tparam Output
 * @tparam Input
 */
template<size_t Dim, class Output, class ...Input> class GridAlgorithm : public Algorithm<
        Output, Input...> {
public:
   typedef CellAlgorithm<Dim, Output, Input...> CellAlgorithmType;

   template<class T, size_t I> struct CellIterator {

      static constexpr void iterate(Output &out, const Input &...in,
                                    std::array<size_t, Dim> &cell,
                                    const std::array<size_t, Dim> &grid_size,
                                    CellAlgorithmType *model) {
         for (cell[I] = 0; cell[I] < grid_size[I]; cell[I]++)
            CellIterator<T, I - 1>::iterate(out, in..., cell, grid_size,
                                            model);
      }

      static constexpr void iterate(Output &out, const Input &...in,
                                    std::array<size_t, Dim> &cell,
                                    const std::array<size_t, Dim> &grid_size,
                                    const std::function<
                                            void(Output &, const Input &...,
                                                 const std::array<size_t, Dim> &,
                                                 const std::array<size_t, Dim> &)> &func) {
         for (cell[I] = 0; cell[I] < grid_size[I]; cell[I]++)
            CellIterator<T, I - 1>::iterate(out, in..., cell, grid_size,
                                            func);
      }
   };

   template<class T>
   struct CellIterator<T, 0> {
      static constexpr size_t I = 0;

      static constexpr void iterate(Output &out, const Input &...in,
                                    std::array<size_t, Dim> &cell,
                                    const std::array<size_t, Dim> &grid_size,
                                    CellAlgorithmType *model) {
         for (cell[I] = 0; cell[I] < grid_size[I]; cell[I]++)
            (*model)(out, in..., cell, grid_size);
      }

      static constexpr void iterate(Output &out, const Input &...in,
                                    std::array<size_t, Dim> &cell,
                                    const std::array<size_t, Dim> &grid_size,
                                    const std::function<
                                            void(Output &, const Input &...,
                                                 const std::array<size_t, Dim> &,
                                                 const std::array<size_t, Dim> &)> &func) {
         for (cell[I] = 0; cell[I] < grid_size[I]; cell[I]++)
            func(out, in..., cell, grid_size);
      }
   };

public:
   GridAlgorithm(CellAlgorithmType *model) :
           model(model) {
      grid_size = QuantityHandler::get()->getValue<RealGrid *>("rgrid")->getGridSize<Dim>();
   }

   virtual void operator()(Output &out, const Input &...in) override {
      std::array<size_t, Dim> cell = {0};
      CellIterator<void, Dim - 1>::iterate(out, in..., cell, grid_size, model);
   }

private:
   CellAlgorithmType *model;
   std::array<size_t, Dim> grid_size;
};

}

#endif /* ALGORITHM_GRID_HH_ */
