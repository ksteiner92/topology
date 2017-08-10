//
// Created by klaus on 09.07.17.
//

#include "rgrid.h"

using namespace std;

namespace ks {

RealGrid::RealGrid(const vector<size_t> &rgrid, const vector<BoundaryCondition> &bc) : rgrid(rgrid), bc(bc) {
}

RealGrid::~RealGrid() = default;

const size_t RealGrid::getSystemDimension() const {
   return rgrid.size();
}

}