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
 * dos.cc
 *
 *  Created on: 20.04.2017
 *      Author: Klaus Steiner
 */

#include "dos.h"

using namespace std;
using namespace Eigen;
using namespace ks_hr;
using namespace ks_sw;

namespace ks {

template<size_t Dim> DensityOfStatesModel<Dim>::DensityOfStatesModel() {
   tb_alg = new TightBindingAlgorithm<Dim>();
   grid_alg = new GridAlgorithm<Dim, MatrixXcd, Vector3d>(tb_alg);
   QuantityHandler *quanHandler = QuantityHandler::get();
   size = getGridVolume<Dim>(quanHandler->getValue<RealGrid *>("rgrid")->getGridSize<Dim>())
          * quanHandler->getValue<TightBindingHamiltonian *>("hr")->getNumAtoms();
   kpath = quanHandler->getValue<KPathBase *>("kpath");
   sw = quanHandler->getValue<SelfEnergyBase *>("sw");
}

template<size_t Dim> DensityOfStatesModel<Dim>::~DensityOfStatesModel() {
   delete tb_alg;
   delete grid_alg;
}

template<size_t Dim> void DensityOfStatesModel<Dim>::operator()(
        ostream &output) {
   map<double, double> dos;
   vector<double> wk;
   const size_t num_kpoints = kpath->getNumKPoints();
   for (size_t i = 0; i < num_kpoints; i++) {
      MatrixXcd H = MatrixXcd::Zero(size, size);
      const string kname = kpath->getPointName(i);
      if (!kname.empty())
         cout << "Calculating bs for point " << kname << endl;
      const Vector3d k = (*kpath)[i];
      (*grid_alg)(H, k);
      SelfAdjointEigenSolver<MatrixXcd> es(H, EigenvaluesOnly);
      const VectorXd &e = es.eigenvalues();
      for (size_t j = 0; j < size; j++)
         wk.push_back(e(j));
   }
   const size_t num_energies = sw->getNumEnergies();
   const double wmax = sw->getEnergyAt(num_energies - 1);
   const double wmin = sw->getEnergyAt(0);
   for (size_t i = 0; i < (num_energies - 1); i++) {
      const double w1 = sw->getEnergyAt(i);
      const double w2 = sw->getEnergyAt(i + 1);
      const long ecount = count_if(wk.begin(), wk.end(), [w1, w2](double j) { return (j < w2) && (w1 <= j); });
      dos.insert(make_pair(w1, (double) num_energies * ecount / (num_kpoints + 1)
                                   / size / (wmax - wmin)));
   }
   for (const auto dosk : dos)
      output << dosk.first << " " << dosk.second << endl;
}

REGISTER_MODEL_ALL_DIMS(DensityOfStatesModel, dos)

} /* namespace ks */
