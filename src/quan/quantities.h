//
// Created by klaus on 09.07.17.
//

#ifndef QUANTITIES_H
#define QUANTITIES_H

#include "quantityhandler.h"
#include "parser/hr/tbh.h"
#include "parser/sw/selfenergy.h"
#include "parser/kpath/kpath.h"
#include "anasw.h"
#include "selfenergybase.h"
#include "kpathbase.h"
#include "algorithm/cell.hh"
#include "rgrid.h"

namespace ks {

class SelfEnergyQuantity : public Quantity<SelfEnergyBase *> {
public:
   ~SelfEnergyQuantity() override;

   bool init(ks_conf::Config *config) override;
};

class TightBindingHamiltonianQuantity : public Quantity<ks_hr::TightBindingHamiltonian *> {
public:
   ~TightBindingHamiltonianQuantity() override;

   bool init(ks_conf::Config *config) override;
};

class RealGridQuantity : public Quantity<RealGrid *> {
public:
   ~RealGridQuantity() override;

   bool init(ks_conf::Config *config) override;
};

class KPathQuantity : public Quantity<KPathBase *> {
public:
   ~KPathQuantity() override;

   bool init(ks_conf::Config *config) override;
};

}

#endif //QUANTITIES_H
