/*
 * z2model.h
 *
 *  Created on: 26.04.2017
 *      Author: klaus
 */

#ifndef MODEL_Z2_H_
#define MODEL_Z2_H_

#include <complex>
#include <vector>

#include "modelfactory.h"
#include "model.hh"
#include "topinv.hh"

namespace ks {

template<size_t Dim> class Z2Model : public TopologicalInvariantBaseModel<Dim> {
   typedef TopologicalInvariantBaseModel<Dim> super;
MODEL(Z2Model)
public:
   Z2Model();

   void operator()(std::ostream &output) override;

private:
   Eigen::MatrixXcd NSy;

   void calcZ2ForPlain(const std::array<size_t, 3> &plain, double offset,
                       std::vector</*int*/double> &z2) const;

   inline const std::complex<double> U(Eigen::MatrixXcd &V, Eigen::MatrixXcd &V_mu, size_t band) const;

};

} /* namespace ks */

#endif /* MODEL_Z2_H_ */
