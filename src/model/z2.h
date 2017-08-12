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
 * z2.h
 *
 *  Created on: 26.04.2017
 *      Author: Klaus Steiner
 */

#ifndef MODEL_Z2_H_
#define MODEL_Z2_H_

#include <complex>
#include <vector>

#include "modelfactory.h"
#include "model.hh"
#include "topinv.hh"

namespace ks {

template<size_t Dim> class Z2Model : public TopologicalInvariantBaseModel<Dim> {
   typedef TopologicalInvariantBaseModel<Dim> super;
MODEL(Z2Model)
public:
   Z2Model();

   void operator()(std::ostream &output) override;

private:
   Eigen::MatrixXcd NSy;

   void calcZ2ForPlain(const std::array<size_t, 3> &plain, double offset,
                       std::vector</*int*/double> &z2) const;

   inline const std::complex<double> U(Eigen::MatrixXcd &V, Eigen::MatrixXcd &V_mu, size_t band) const;

};

} /* namespace ks */

#endif /* MODEL_Z2_H_ */
