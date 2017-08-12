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
 * kpath.h
 *
 *  Created on: 18.06.2017
 *      Author: Klaus Steiner
 */

#ifndef KPATHBASE_H_
#define KPATHBASE_H_

#include <string>
#include <vector>
#include <map>

#include "Eigen/Eigenvalues"
#include "Eigen/Dense"

namespace ks {

class KPathBase {
public:
   const size_t getNumKPoints() const;

   const Eigen::Vector3d operator[](size_t idx) const;

   const std::string getPointName(size_t idx) const;

   void setBZ(const Eigen::Matrix3d &bz);

   void addPoint(const Eigen::Vector3d &point);

   void addSymmPoint(const std::string &name, const Eigen::Vector3d &point);

protected:
   Eigen::Matrix3d bz;
   std::vector<Eigen::Vector3d> points;
   std::map<size_t, std::string> symm_points;
};

}

#endif /* KPATHBASE_H_ */
