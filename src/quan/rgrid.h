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
 */
//
// Created by klaus on 09.07.17.
//

#ifndef QUAN_RGRID_H
#define QUAN_RGRID_H

#include <vector>

#include "algorithm/cell.hh"

namespace ks {

class RealGrid {
public:
   RealGrid(const std::vector<size_t> &rgrid, const std::vector<BoundaryCondition> &bc);

   RealGrid(const RealGrid &) = default;

   virtual ~RealGrid();

   template<size_t Dim> const std::array<size_t, Dim> getGridSize() const {
      std::array<size_t, Dim> grid_size = {1};
      for (size_t i = 0; i < rgrid.size(); i++)
         grid_size[i] = rgrid[i];
      return grid_size;
   }

   template<size_t Dim> const std::array<BoundaryCondition, Dim> getBoundaryConditions() const {
      std::array<BoundaryCondition, Dim> bc = {BoundaryCondition::PERIODIC};
      for (size_t i = 0; i < this->bc.size(); i++)
         bc[i] = this->bc[i];
      return bc;
   }

   const size_t getSystemDimension() const;

private:
   std::vector<size_t> rgrid;
   std::vector<BoundaryCondition> bc;
};

}

#endif //QUAN_RGRID_H
