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
 * dos.h
 *
 *  Created on: 20.04.2017
 *      Author: Klaus Steiner
 */

#ifndef MODEL_DOS_H_
#define MODEL_DOS_H_

#include <array>
#include <map>

#include "modelfactory.h"
#include "model.hh"
#include "algorithm/tb.h"
#include "algorithm/grid.hh"
#include "parser/sw/selfenergy.h"
#include "parser/hr/tbh.h"
#include "model/bs.h"
#include "quan/kpathbase.h"

namespace ks {

template<size_t Dim> class DensityOfStatesModel : public Model {
MODEL(DensityOfStatesModel)
public:
   DensityOfStatesModel();

   ~DensityOfStatesModel();

   void operator()(std::ostream &output) override;

private:
   GridAlgorithm<Dim, Eigen::MatrixXcd, Eigen::Vector3d> *grid_alg;
   TightBindingAlgorithm<Dim> *tb_alg;
   KPathBase *kpath;
   SelfEnergyBase *sw;
   size_t size;
};

} /* namespace ks */

#endif /* MODEL_DOS_H_ */
