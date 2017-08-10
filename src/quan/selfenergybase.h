/*
 * selfenergybase.h
 *
 *  Created on: 25.06.2017
 *      Author: klaus
 */

#ifndef SELFENERGYBASE_H_
#define SELFENERGYBASE_H_

#include <iostream>
#include <vector>
#include <complex>

namespace ks {

class SelfEnergyBase {
public:
   virtual ~SelfEnergyBase() {
   }

   virtual const size_t getNumEnergies() const = 0;

   virtual const double getEnergyAt(const size_t idx) const = 0;

   virtual const std::vector<std::complex<double>> &operator[](
           const size_t idx) const = 0;

};

}

#endif /* SELFENERGYBASE_H_ */
