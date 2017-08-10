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
