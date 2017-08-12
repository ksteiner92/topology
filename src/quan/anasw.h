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
 * analyticalselfenergy.h
 *
 *  Created on: 25.06.2017
 *      Author: Klaus Steiner
 */

#ifndef ANALYTICALSELFENERGY_H_
#define ANALYTICALSELFENERGY_H_

#include <complex>
#include <functional>

#include "selfenergybase.h"

namespace ks {

class AnalyticalSelfEnergy : public SelfEnergyBase {
public:
   AnalyticalSelfEnergy(double w_min, double w_max,
                        size_t dens, size_t num_atoms);

   void setFunction(const std::function<std::complex<double>(double)> &sigma);

   const size_t getNumEnergies() const override;

   const double getEnergyAt(size_t idx) const override;

   const std::vector<std::complex<double>> &operator[](size_t idx) const override;

private:
   double w_min;
   double w_max;
   double dw;
   size_t dens;
   size_t num_atoms;
   std::function<std::complex<double>(double)> sigma;
};

} /* namespace ks */

#endif /* ANALYTICALSELFENERGY_H_ */
