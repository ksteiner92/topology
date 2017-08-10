/*
 * analyticalselfenergy.h
 *
 *  Created on: 25.06.2017
 *      Author: klaus
 */

#ifndef ANALYTICALSELFENERGY_H_
#define ANALYTICALSELFENERGY_H_

#include <complex>
#include <functional>

#include "selfenergybase.h"

namespace ks {

class AnalyticalSelfEnergy : public SelfEnergyBase {
public:
   AnalyticalSelfEnergy(double w_min, double w_max,
                        size_t dens, size_t num_atoms);

   void setFunction(const std::function<std::complex<double>(double)> &sigma);

   const size_t getNumEnergies() const override;

   const double getEnergyAt(size_t idx) const override;

   const std::vector<std::complex<double>> &operator[](size_t idx) const override;

private:
   double w_min;
   double w_max;
   double dw;
   size_t dens;
   size_t num_atoms;
   std::function<std::complex<double>(double)> sigma;
};

} /* namespace ks */

#endif /* ANALYTICALSELFENERGY_H_ */
