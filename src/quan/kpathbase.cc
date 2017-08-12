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

#include "kpathbase.h"

using namespace std;
using namespace Eigen;

namespace ks {

const size_t KPathBase::getNumKPoints() const {
   return points.size();
}

void KPathBase::setBZ(const Matrix3d &bz) {
   this->bz = bz;
}

const Vector3d KPathBase::operator[](size_t idx) const {
   return points[idx].transpose() * bz;
}

const std::string KPathBase::getPointName(size_t idx) const {
   const auto it = symm_points.find(idx);
   if (it != symm_points.end())
      return it->second;
   return "";
}

void KPathBase::addPoint(const Vector3d &point) {
   points.push_back(point);
}

void KPathBase::addSymmPoint(const string &name, const Vector3d &point) {
   points.push_back(point);
   symm_points.insert(pair<size_t, string>(points.size() - 1, name));
}

}