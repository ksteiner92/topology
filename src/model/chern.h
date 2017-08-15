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
 * chern.h
 *
 *  Created on: 23.04.2017
 *      Author: Klaus Steiner
 */

#ifndef MODEL_CHERN_H_
#define MODEL_CHERN_H_

#include <array>
#include <complex>
#include <vector>

#include "modelfactory.h"
#include "model.hh"
#include "topinv.hh"

namespace ks {

template<size_t Dim> class ChernModel : public TopologicalInvariantBaseModel<Dim> {
   typedef TopologicalInvariantBaseModel<Dim> super;
MODEL(ChernModel)
public:
   void operator()(std::ostream &output) override;
};

} /* namespace ks */

#endif /* MODEL_CHERN_H_ */
