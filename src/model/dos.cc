/*
 * dosmodel.cc
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
      long ecount = count_if(wk.begin(), wk.end(), [w1, w2](double j) { return (j < w2) && (w1 <= j); });
      dos.insert(
              pair<double, double>(w1,
                                   (double) num_energies * ecount / (num_kpoints + 1)
                                   / size / (wmax - wmin)));
   }
   for (const auto dosk : dos)
      output << dosk.first << " " << dosk.second << endl;

}

REGISTER_MODEL_ALL_DIMS(DensityOfStatesModel, dos)

} /* namespace ks */
