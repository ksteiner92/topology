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
 * selfenergybase.h
 *
 *  Created on: 25.06.2017
 *      Author: Klaus Steiner
 */

#ifndef SELFENERGYBASE_H_
#define SELFENERGYBASE_H_

#include <iostream>
#include <vector>
#include <complex>

namespace ks {

class SelfEnergyBase {
public:
   virtual const size_t getNumEnergies() const = 0;

   virtual const double getEnergyAt(const size_t idx) const = 0;

   virtual const std::vector<std::complex<double>> &operator[](
           const size_t idx) const = 0;

};

}

#endif /* SELFENERGYBASE_H_ */
