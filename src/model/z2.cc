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
 * z2.cc
 *
 *  Created on: 26.04.2017
 *      Author: Klaus Steiner
 */

#include "z2.h"

using namespace std;
using namespace ks_hr;
using namespace ks_k;
using namespace Eigen;

namespace ks {

template<size_t Dim> Z2Model<Dim>::Z2Model() {
   if (super::size % 2 != 0)
      throw logic_error("Number of bands must be even!");
   NSy = MatrixXcd::Zero(super::size, super::size);
   for (size_t i = 0; i < super::size / 2; i++) {
      NSy(i, i + super::size / 2) = -1i;
      NSy(i + super::size / 2, i) = 1i;
   }
}

template<size_t Dim> const complex<double> Z2Model<Dim>::U(
        MatrixXcd &V, MatrixXcd &V_mu,
        const size_t band) const {
   MatrixXcd psi = V.block(0, band, super::num_atoms, 2);
   const MatrixXcd psi1 = V_mu.block(0, band, super::num_atoms, 2);
   const Matrix2cd tmp1 = psi.adjoint() * psi1;
   complex<double> U1k = tmp1(0, 0) * tmp1(1, 1) - tmp1(1, 0) * tmp1(0, 1);
   U1k /= abs(U1k);
   return U1k;
}

template<size_t Dim> void Z2Model<Dim>::calcZ2ForPlain(
        const array<size_t, 3> &plain, const double offset,
        vector</*int*/double> &z2) const {
   const double k_min = -0.5;
   const double k_max = 0.5;
   const double dk = (k_max - k_min) / (super::kdens - 1);
   const size_t half_size = super::kdens / 2;
   Vector3d k;
   k[plain[2]] = offset;
   const size_t B_minus_vol = super::kdens * (super::kdens / 2 + 1);
   vector<MatrixXcd> eigenv(B_minus_vol);
   const size_t num_bands = super::size;

   /* The t4 point is here denoted as the origin in the k-mesh on the boundary
    * between B^- and B^+. The constraint |n(-k_l)> = T|n(k_l)> can only applied
    * on this boundary and the states are mirrored along the point t4. */
   const size_t t4idx = half_size * super::kdens + half_size;
   const size_t t2idx = half_size;
   /* Iterate over all grid points or their indices */
   for (size_t i = 0; i < B_minus_vol; i++) {
      /* We found points which we constrain with |n(-k_l)> = T|n(k_l)> */
      size_t tidx;
      bool applytrev = false;
      if (i > t4idx && i < (B_minus_vol - 1)) {
         tidx = t4idx;
         applytrev = true;
      } else if (i > t2idx && i < (super::kdens - 1)) {
         tidx = t2idx;
         applytrev = true;
      }
      if (applytrev) {
         const size_t cpidx = tidx + tidx - i;
         MatrixXcd V = eigenv[cpidx];
         for (size_t band = 0; band < num_bands; band++)
            V.col(band) = 1i * NSy * V.col(band).conjugate();
         eigenv[i] = V;
      }
         /* We found a point which is already determinate by the periodic BC,
          * so just copy */
      else if (((i + 1) % super::kdens) == 0) {
         const size_t cpidx = i - (super::kdens - 1);
         eigenv[i] = eigenv[cpidx];
      }
         /* Ok, here we actually have to solve the eigen value problem */
      else {
         k[plain[0]] = super::getKGridPoint(k_min, k_max, dk, i / super::kdens,
                                            super::kdens);
         k[plain[1]] = super::getKGridPoint(k_min, k_max, dk, i % super::kdens,
                                            super::kdens);
         k = k.transpose() * super::bz;
         /* check if we are on a time reverse invariant point
          * (four are relevant). */
         const bool trevp = (i == 0 || i == half_size
                             || i == (half_size * super::kdens)
                             || i == (half_size * super::kdens + half_size));
         MatrixXcd H = MatrixXcd::Zero(super::size, super::size);
         (*super::grid_alg)(H, k);
         SelfAdjointEigenSolver<MatrixXcd> es(H, ComputeEigenvectors);
         MatrixXcd V = es.eigenvectors();
         /* We are on a time reverse invariant point, let's apply the
          * constraint: |2(n + 1)(k_l)> = T|(2n + 1)(k_l)> */
         if (trevp) {
            for (size_t band = 2; band <= num_bands; band *= 2)
               V.col(band - 1) = 1i * NSy * V.col(band - 2).conjugate();
         }
         eigenv[i] = V;
      }
   }
   /* Calculate the link variables and Z2 invariants out of it */
   const size_t num_non_deg_bands = num_bands / 2;
   complex<double> Z2(0.0, 0.0);
   for (size_t band = 0; band < num_non_deg_bands; band += 2) {
      for (size_t j = 0; j < super::kdens / 2; j++) {
         for (size_t i = 1; i < super::kdens; i++) {
            const size_t idx0 = j * super::kdens + i - 1;
            MatrixXcd V_0 = eigenv[idx0];
            MatrixXcd V_1 = eigenv[idx0 + 1];
            MatrixXcd V_2 = eigenv[idx0 + 1 + super::kdens];
            MatrixXcd V_3 = eigenv[idx0 + super::kdens];
            Z2 -= log(
                    U(V_0, V_1, band) * U(V_1, V_2, band)
                    / U(V_3, V_2, band) / U(V_0, V_3, band));
         }
      }
      for (size_t kidx = 0; kidx < (super::kdens - 1); kidx++) {
         const size_t b1 = kidx;
         const size_t b2 = half_size * super::kdens + kidx;
         Z2 += log(U(eigenv[b1], eigenv[b1 + 1], band))
               - log(U(eigenv[b2], eigenv[b2 + 1], band));
      }
      z2.push_back(real(Z2 / (2.0 * M_PI * 1i)));
   }
}

template<> void Z2Model<1>::operator()(std::ostream &output) {
   throw logic_error("Z2 model for dimension 1 not supported");
}

template<> void Z2Model<2>::operator()(std::ostream &output) {
   vector<array<double, 1>> z2_nums;
   vector</*int*/double> z2;
   array<size_t, 3> plain = {0, 1, 2};
   calcZ2ForPlain(plain, 0.0, z2);
   for (size_t band = 0; band < size / 2; band++) {
      array<double, 1> nu = {z2[band]};
      z2_nums.push_back(nu);
   }
}

template<> void Z2Model<3>::operator()(std::ostream &output) {
   vector<array<double, 4>> z2_nums;
   const size_t num_bands = size / 2;
   array<size_t, 3> plain = {1, 2, 0};
   vector</*int*/double> z2_x0;
   calcZ2ForPlain(plain, 0.0, z2_x0);
   cout << "Cylinder 1:" << endl;
   for (size_t band = 0; band < num_bands; band++) {
      cout << "\tband " << (band + 1) << ": " << z2_x0[band] << endl;
   }
   vector</*int*/double> z2_xpi;
   calcZ2ForPlain(plain, M_PI, z2_xpi);
   cout << "Cylinder 2:" << endl;
   for (size_t band = 0; band < num_bands; band++) {
      cout << "\tband " << (band + 1) << ": " << z2_xpi[band] << endl;
   }
   vector</*int*/double> z2_ypi;
   plain = {0, 2, 1};
   calcZ2ForPlain(plain, M_PI, z2_ypi);
   cout << "Cylinder 3:" << endl;
   for (size_t band = 0; band < num_bands; band++) {
      cout << "\tband " << (band + 1) << ": " << z2_ypi[band] << endl;
   }
   vector</*int*/double> z2_zpi;
   plain = {0, 1, 2};
   calcZ2ForPlain(plain, M_PI, z2_zpi);
   cout << "Cylinder 4:" << endl;
   for (size_t band = 0; band < num_bands; band++) {
      cout << "\tband " << (band + 1) << ": " << z2_zpi[band] << endl;
   }
   for (size_t band = 0; band < num_bands; band++) {
      array<double, 4> nu;
      nu[0] = z2_x0[band];
      nu[1] = z2_xpi[band];
      nu[2] = z2_ypi[band];
      nu[3] = z2_zpi[band];
      z2_nums.push_back(nu);
   }
   cout << "dumping Z2 numbers..." << endl;
   for (size_t i = 0; i < 4; i++) {
      output << "Cylinder " << i + 1 << ":" << endl;
      for (size_t band = 0; band < z2_nums.size(); band++) {
         output << "band " << band + 1 << ": "
                << round(abs(remainder(z2_nums[band][i], 2))) << " "
                << "numerical value: " << z2_nums[band][i] << endl;
      }
   }
   output << "Summery:" << endl;
   for (size_t band = 0; band < z2_nums.size(); band++) {
      output << "band " << band + 1 << ": "
             << round(abs(remainder(z2_nums[band][0], 2)))
                * round(abs(remainder(z2_nums[band][1], 2)))
             << "(" << round(abs(remainder(z2_nums[band][1], 2)))
             << ";" << round(abs(remainder(z2_nums[band][2], 2)))
             << ";" << round(abs(remainder(z2_nums[band][3], 2)))
             << ")" << endl;
   }
}

REGISTER_MODEL_ALL_DIMS(Z2Model, Z2)

} /* namespace ks */
