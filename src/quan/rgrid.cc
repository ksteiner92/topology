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