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
 * analyticalselfenergy.cc
 *
 *  Created on: 25.06.2017
 *      Author: Klaus Steiner
 */

#include "anasw.h"

using namespace std;

namespace ks {

AnalyticalSelfEnergy::AnalyticalSelfEnergy(double w_min, double w_max, size_t dens, size_t num_atoms) :
        w_min(w_min), w_max(w_max), dens(dens), num_atoms(num_atoms) {
   dw = (w_max - w_min) / dens;
}

void AnalyticalSelfEnergy::setFunction(const function<complex<double>(double)> &sigma) {
   this->sigma = sigma;
}

const size_t AnalyticalSelfEnergy::getNumEnergies() const {
   return dens;
}

const double AnalyticalSelfEnergy::getEnergyAt(size_t idx) const {
   return w_min + dw * idx;
}

const vector<complex<double>> &AnalyticalSelfEnergy::operator[](size_t idx) const {
   vector<complex<double>> *res = new vector<complex<double>>(num_atoms,
                                                              sigma(getEnergyAt(idx)));
   return *res;
}

}
/* namespace ks */
