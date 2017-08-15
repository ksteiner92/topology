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
 * chern.cc
 *
 *  Created on: 23.04.2017
 *      Author: Klaus Steiner
 */

#include "chern.h"

using namespace std;
using namespace ks_hr;
using namespace ks_k;
using namespace Eigen;

namespace ks {

template<size_t Dim> void ChernModel<Dim>::operator()(std::ostream &output) {
   throw logic_error("Chern model for dimension != 2 not yet implemented");
}

template<> void ChernModel<2>::operator()(ostream &output) {
   const double k_min = -M_PI;
   const double k_max = M_PI;
   const double dk = (k_max - k_min) / (super::kdens - 1);
   const size_t B_s_minus_vol = super::kdens * (super::kdens / 2 + 1);
   vector<int> chern_nums;
   vector<complex<double>> U1;
   vector<complex<double>> U2;
   vector<MatrixXcd> eigenv;
   Vector3d k;
   k[2] = 0.0;
   for (size_t i = 0; i < B_s_minus_vol; i++) {
      k[0] = getKGridPoint(k_min, k_max, dk, i / super::kdens, super::kdens);
      k[1] = getKGridPoint(k_min, k_max, dk, i % super::kdens, super::kdens);
      k = k.transpose() * super::bz;
      MatrixXcd H = MatrixXcd::Zero(super::size, super::size);
      (*super::grid_alg)(H, k);
      SelfAdjointEigenSolver<MatrixXcd> es(H, ComputeEigenvectors);
      eigenv.push_back(es.eigenvectors());
   }

   for (size_t kiy = 0; kiy < super::kdens / 2; kiy++) {
      for (size_t kix = 1; kix < super::kdens; kix++) {
         for (size_t band = 0; band < super::size; band++) {
            const size_t idx0 = kiy * super::kdens + kix - 1;
            const RowVectorXcd Uk0_band = eigenv[idx0].col(band).adjoint();
            const VectorXcd Uk1_band = eigenv[idx0 + 1].col(band);
            const VectorXcd Uk2_band = eigenv[idx0 + super::kdens].col(band);
            complex<double> overlap = (Uk0_band * Uk1_band)(0, 0);
            overlap /= abs(overlap);
            U1.push_back(overlap);
            overlap = (Uk0_band * Uk2_band)(0, 0);
            overlap /= abs(overlap);
            U2.push_back(overlap);
         }
      }
   }
   for (size_t band = 0; band < super::size; band++) {
      complex<double> chern(0.0, 0.0);
      for (size_t kiy = 0; kiy < super::kdens / 2; kiy++) {
         for (size_t kix = 1; kix < super::kdens; kix++) {
            const size_t idx0 = kiy * super::kdens + kix - 1;
            chern += log(
                    U1[idx0 * super::size + band]
                    * U2[(idx0 + 1) * super::size + band]
                    / U1[(idx0 + super::kdens) * super::size + band]
                    / U2[idx0 * super::size + band]);
         }
      }
      chern_nums.push_back(
              round(abs(remainder(real(chern / (2.0 * M_PI * 1i)), 2))));
   }
   for (size_t band = 0; band < chern_nums.size(); band++)
      output << band + 1 << " " << chern_nums[band] << endl;
}

REGISTER_MODEL_ALL_DIMS(ChernModel, chern)

} /* namespace ks */
