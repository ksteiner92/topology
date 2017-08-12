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
 * aw.h
 *
 *  Created on: 18.04.2017
 *      Author: Klaus Steiner
 */

#ifndef MODEL_AW_H_
#define MODEL_AW_H_

#include <array>
#include <complex>
#include <vector>

#include "modelfactory.h"
#include "model.hh"
#include "quan/kpathbase.h"
#include "quan/selfenergybase.h"
#include "quan/rgrid.h"
#include "algorithm/grid.hh"
#include "algorithm/tb.h"
#include "algorithm/sw.h"
#include "parser/hr/tbh.h"
#include "Eigen/Eigenvalues"
#include "Eigen/Dense"

namespace ks {

template<size_t Dim> class BaseSpectralFunctionModel : public Model {
public:
   BaseSpectralFunctionModel();

   ~BaseSpectralFunctionModel();

   void operator()(std::ostream &output) override;

protected:
   double idelta;
   size_t num_atoms;
   SelfEnergyBase *sw;
   KPathBase *kpath;
   TightBindingAlgorithm<Dim> *tb_alg;
   GridAlgorithm<Dim, Eigen::MatrixXcd, Eigen::Vector3d> *grid_alg_tb;
   GridAlgorithm<Dim, Eigen::MatrixXcd, size_t> *grid_alg_sw;
   SelfEnergyAlgorithm<Dim> *sw_alg;
   size_t size;

   virtual const double calculate(const Eigen::MatrixXcd &C) const = 0;

   virtual void dump(const std::vector<std::vector<std::array<double, 2>>> &Awk, std::ostream &output) const = 0;
};

template<size_t Dim> class SpectralFunctionModel_AW_K : public BaseSpectralFunctionModel<
        Dim> {
MODEL(SpectralFunctionModel_AW_K)
   typedef BaseSpectralFunctionModel<Dim> super;
public:
   ~SpectralFunctionModel_AW_K() = default;

protected:
   inline const double calculate(const Eigen::MatrixXcd &C) const override;

   inline void dump(const std::vector<std::vector<std::array<double, 2>>> &Awk, std::ostream &output) const override;

};

template<size_t Dim> class SpectralFunctionModel_AW_Edge : public BaseSpectralFunctionModel<
        Dim> {
MODEL(SpectralFunctionModel_AW_Edge)
   typedef BaseSpectralFunctionModel<Dim> super;
public:
   ~SpectralFunctionModel_AW_Edge() = default;

protected:
   inline const double calculate(const Eigen::MatrixXcd &C) const override;

   inline void dump(const std::vector<std::vector<std::array<double, 2>>> &Awk, std::ostream &output) const override;

};

template<size_t Dim> class SpectralFunctionModel_AW_Verbose : public BaseSpectralFunctionModel<
        Dim> {
MODEL(SpectralFunctionModel_AW_Verbose)
   typedef BaseSpectralFunctionModel<Dim> super;
public:
   ~SpectralFunctionModel_AW_Verbose() = default;

protected:
   inline const double calculate(const Eigen::MatrixXcd &C) const override;

   inline void dump(const std::vector<std::vector<std::array<double, 2>>> &Awk, std::ostream &output) const override;

};

template<size_t Dim> class SpectralFunctionModel_AW : public SpectralFunctionModel_AW_K<
        Dim> {
MODEL(SpectralFunctionModel_AW)
   typedef SpectralFunctionModel_AW_K<Dim> super;
public:
   ~SpectralFunctionModel_AW() = default;

protected:
   inline void dump(const std::vector<std::vector<std::array<double, 2>>> &Awk, std::ostream &output) const override;

};

}

#endif /* MODEL_AW_H_ */
