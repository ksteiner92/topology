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
 * aw.cc
 *
 *  Created on: 20.04.2017
 *      Author: Klaus Steiner
 */

#include "aw.h"

using namespace std;
using namespace Eigen;
using namespace ks_hr;

namespace ks {

template<size_t Dim> BaseSpectralFunctionModel<Dim>::BaseSpectralFunctionModel() {
   tb_alg = new TightBindingAlgorithm<Dim>();
   sw_alg = new SelfEnergyAlgorithm<Dim>();
   grid_alg_tb = new GridAlgorithm<Dim, MatrixXcd, Vector3d>(tb_alg);
   grid_alg_sw = new GridAlgorithm<Dim, MatrixXcd, size_t>(sw_alg);
   QuantityHandler *quanHandler = QuantityHandler::get();
   const array<size_t, Dim> grid_size = quanHandler->getValue<RealGrid *>("rgrid")->getGridSize<Dim>();
   num_atoms = quanHandler->getValue<TightBindingHamiltonian *>("hr")->getNumAtoms();
   size = getGridVolume<Dim>(grid_size) * num_atoms;
   kpath = quanHandler->getValue<KPathBase *>("kpath");
   idelta = quanHandler->getValue<double>("idelta");
   sw = quanHandler->getValue<SelfEnergyBase *>("sw");
}

template<size_t Dim> BaseSpectralFunctionModel<Dim>::~BaseSpectralFunctionModel() {
   delete tb_alg;
   delete sw_alg;
   delete grid_alg_tb;
   delete grid_alg_sw;
}

template<size_t Dim> void BaseSpectralFunctionModel<Dim>::operator()(
        ostream &output) {
   const size_t num_kpoints = kpath->getNumKPoints();
   vector<vector<array<double, 2>>> Awk(num_kpoints);
   size_t num_threads = 1;
#ifdef _OPENMP
#pragma omp parallel
   {
#pragma omp single
       num_threads = omp_get_num_threads();
   }
#endif
   const double tenth = (double) num_kpoints / 10.0 / (double) num_threads;
   int offset = -1;
   int last_progress_print = -1;
#pragma omp parallel for firstprivate(offset, last_progress_print)
   for (size_t i = 0; i < num_kpoints; i++) {
      if (offset < 0)
         offset = i;
      const int progress = (i - offset) / tenth;
#ifdef _OPENMP
      const size_t thread = omp_get_thread_num();
#endif
      if (last_progress_print < 0 || progress > last_progress_print) {
         last_progress_print = progress;
         stringstream ss;
#ifdef _OPENMP
         ss << "Thread " << thread << " ";
#endif
         ss << progress * 10 << "%" << endl;
         cout << ss.str();
      }
      const Vector3d k = (*kpath)[i];
      MatrixXcd H = MatrixXcd::Zero(size, size);
      (*grid_alg_tb)(H, k);
      VectorXcd sw_omega = VectorXcd::Zero(num_atoms);
      vector<array<double, 2>> Ak;
      for (size_t j = 0; j < sw->getNumEnergies(); j++) {
         array<double, 2> Aw;
         Aw[0] = sw->getEnergyAt(j);
         const complex<double> iw(Aw[0], idelta);
         MatrixXcd sigma = MatrixXcd::Zero(size, size);
         (*grid_alg_sw)(sigma, j);
         MatrixXcd C =
                 (iw * MatrixXcd::Identity(size, size) - H - sigma).inverse();
         Aw[1] = calculate(C);
         Ak.push_back(Aw);
      }
      Awk[i] = Ak;
   }
   dump(Awk, output);
}

template<size_t Dim> const double SpectralFunctionModel_AW_K<Dim>::calculate(const MatrixXcd &C) const {
   const size_t num_kpoints = super::kpath->getNumKPoints();
   return -imag(C.trace()) / M_PI / (num_kpoints + 1) / super::size;
}

template<size_t Dim> void SpectralFunctionModel_AW_K<Dim>::dump(
        const vector<vector<array<double, 2>>> &Awk, std::ostream &output) const {
   cout << "dumping spectral function..." << endl;
   const size_t num_kpoints = super::kpath->getNumKPoints();
   for (size_t i = 0; i < num_kpoints; i++) {
      for (const array<double, 2> Aw : Awk[i])
         output << i << " " << Aw[0] << " " << Aw[1] << endl;
      output << endl;
   }
}

template<size_t Dim> void SpectralFunctionModel_AW_Edge<Dim>::dump(
        const vector<vector<array<double, 2>>> &Awk, std::ostream &output) const {
   cout << "dumping spectral function..." << endl;
   const size_t num_kpoints = super::kpath->getNumKPoints();
   for (size_t i = 0; i < num_kpoints; i++) {
      for (const array<double, 2> Aw : Awk[i])
         output << i << " " << Aw[0] << " " << Aw[1] << endl;
      output << endl;
   }
}

template<size_t Dim> const double SpectralFunctionModel_AW_Edge<Dim>::calculate(const MatrixXcd &C) const {
   const size_t num_kpoints = super::kpath->getNumKPoints();
   complex<double> edgetr(0.0, 0.0);
   for (size_t j = 0; j < super::num_atoms; j++)
      edgetr += C(j, j) + C(super::size - j - 1, super::size - j - 1);
   return -imag(edgetr) / M_PI / (num_kpoints + 1) / super::size;
}

template<size_t Dim> const double SpectralFunctionModel_AW_Verbose<Dim>::calculate(const MatrixXcd &C) const {
   const size_t num_kpoints = super::kpath->getNumKPoints();
   cout << " "
        << -C.block(0, 0, super::num_atoms, super::num_atoms).diagonal().imag()
           / M_PI / (num_kpoints + 1) / super::size;
   cout << " "
        << -C.block(super::size - super::num_atoms - 1,
                    super::size - super::num_atoms - 1, super::num_atoms,
                    super::num_atoms).diagonal().imag() / M_PI
           / (num_kpoints + 1) / super::size;
   return -imag(C.trace()) / M_PI / (num_kpoints + 1) / super::size;
}

template<size_t Dim> void SpectralFunctionModel_AW_Verbose<Dim>::dump(
        const vector<vector<array<double, 2>>> &Awk, std::ostream &output) const {
}

template<size_t Dim> void SpectralFunctionModel_AW<Dim>::dump(
        const vector<vector<array<double, 2>>> &Awk, std::ostream &output) const {
   cout << "integrating spectral function..." << endl;
   const size_t num_kpoints = super::kpath->getNumKPoints();
   vector<double> Aw(super::sw->getNumEnergies());
   fill(Aw.begin(), Aw.end(), 0.0);
   for (size_t i = 0; i < num_kpoints; i++)
      for (size_t j = 0; j < Awk[i].size(); j++)
         Aw[j] += Awk[i][j][1];
   cout << "dumping integrated spectral function..." << endl;
   for (size_t i = 0; i < Aw.size(); i++)
      output << super::sw->getEnergyAt(i) << " " << Aw[i] << endl;
}

REGISTER_MODEL_ALL_DIMS(SpectralFunctionModel_AW_K, Awk)

REGISTER_MODEL_ALL_DIMS(SpectralFunctionModel_AW_Edge, AwEdge)

REGISTER_MODEL_ALL_DIMS(SpectralFunctionModel_AW_Verbose, AwVerbose)

REGISTER_MODEL_ALL_DIMS(SpectralFunctionModel_AW, Aw)

}
