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
 * bs.cc
 *
 *  Created on: 20.04.2017
 *      Author: Klaus Steiner
 */

#include "bs.h"

using namespace std;
using namespace Eigen;
using namespace ks_hr;

namespace ks {

template<size_t Dim> BandStructureModel<Dim>::BandStructureModel() {
   tb_alg = new TightBindingAlgorithm<Dim>();
   grid_alg = new GridAlgorithm<Dim, MatrixXcd, Vector3d>(tb_alg);
   QuantityHandler *quanHandler = QuantityHandler::get();
   size = getGridVolume<Dim>(quanHandler->getValue<RealGrid *>("rgrid")->getGridSize<Dim>())
          * quanHandler->getValue<TightBindingHamiltonian *>("hr")->getNumAtoms();
   kpath = quanHandler->getValue<KPathBase *>("kpath");
}

template<size_t Dim> BandStructureModel<Dim>::~BandStructureModel() {
   delete tb_alg;
   delete grid_alg;
}

template<size_t Dim> void BandStructureModel<Dim>::operator()(
        std::ostream &output) {
   const size_t num_kpoints = kpath->getNumKPoints();
   vector<VectorXd> ek(num_kpoints);
   size_t num_threads = 1;
#ifdef _OPENMP
#pragma omp parallel
   {
#pragma omp single
       num_threads = omp_get_num_threads();
   }
#endif
   const size_t tenth = num_kpoints / 10;
   int offset = -1;
#pragma omp parallel for firstprivate(offset)
   for (size_t i = 0; i < num_kpoints; i++) {
      MatrixXcd H = MatrixXcd::Zero(size, size);

      const string kname = kpath->getPointName(i);
#ifdef _OPENMP
      if (offset < 0)
      offset = i;
      const size_t thread = omp_get_thread_num();
      const size_t progress = (i - offset) * num_threads;
      if (!kname.empty()) {
           stringstream ss;
           ss << "Thread " << thread << ": " << "Calculating bs for point " << kname << endl;
           cout << ss.str();
      }
      if ((progress % tenth) == 0) {
          stringstream ss;
          ss << "Thread " << thread << ": " << progress / tenth * 10 << "%" << endl;
          cout << ss.str();
      }
#else
      if (!kname.empty())
         cout << "Calculating bs for point " << kname << endl;
      if (((i * num_threads) % tenth) == 0)
         cout << i / tenth * 10 << "%" << endl;
#endif
      const Vector3d k = (*kpath)[i];
      (*grid_alg)(H, k);
      SelfAdjointEigenSolver<MatrixXcd> es(H, EigenvaluesOnly);
      ek[i] = es.eigenvalues();
   }
   IOFormat printFmt(7, DontAlignCols, " ", " ", "", "", "", "");
   for (size_t i = 0; i < num_kpoints; i++)
      output << i << " " << ek[i].transpose().format(printFmt) << endl;
}

template<size_t Dim> RealSwBandStructureModel<Dim>::RealSwBandStructureModel() : BandStructureModel<Dim>() {
   sw_alg = new SelfEnergyAlgorithm<Dim>();
   grid_sw_alg = new GridAlgorithm<Dim, MatrixXcd, size_t>(sw_alg);
}

template<size_t Dim> RealSwBandStructureModel<Dim>::~RealSwBandStructureModel() {
   delete sw_alg;
   delete grid_sw_alg;
}

template<size_t Dim> void RealSwBandStructureModel<Dim>::operator()(
        std::ostream &output) {
   const size_t num_kpoints = super::kpath->getNumKPoints();
   vector<VectorXd> ek(num_kpoints);
   const size_t tenth = num_kpoints / 10;
#ifdef _OPENMP
   const double wt1 = omp_get_wtime();
#endif
#pragma omp parallel for
   for (size_t i = 0; i < num_kpoints; i++) {
      MatrixXcd H = MatrixXcd::Zero(super::size, super::size);
      const string kname = super::kpath->getPointName(i);
      if (!kname.empty())
         cout << "Calculating bs with real self energy for point " << kname
              << endl;
      if ((i % tenth) == 0)
         cout << i / tenth * 10 << "%" << endl;
      const Vector3d k = (*super::kpath)[i];
      (*super::grid_alg)(H, k);
      MatrixXcd sigma = MatrixXcd::Zero(super::size, super::size);
      (*grid_sw_alg)(sigma, 0);
      H = H + sigma;
      SelfAdjointEigenSolver<MatrixXcd> es(H, EigenvaluesOnly);
      ek[i] = es.eigenvalues();
   }
   IOFormat printFmt(7, DontAlignCols, " ", " ", "", "", "", "");
   for (size_t i = 0; i < num_kpoints; i++)
      output << i << " " << ek[i].transpose().format(printFmt) << endl;
}

REGISTER_MODEL_ALL_DIMS(BandStructureModel, bs);

REGISTER_MODEL_ALL_DIMS(RealSwBandStructureModel, ReBs);

}

