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
 */
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
   ~SelfEnergyQuantity();

   bool init(ks_conf::Config *config) override;
};

class TightBindingHamiltonianQuantity : public Quantity<ks_hr::TightBindingHamiltonian *> {
public:
   ~TightBindingHamiltonianQuantity();

   bool init(ks_conf::Config *config) override;
};

class RealGridQuantity : public Quantity<RealGrid *> {
public:
   ~RealGridQuantity();

   bool init(ks_conf::Config *config) override;
};

class KPathQuantity : public Quantity<KPathBase *> {
public:
   ~KPathQuantity();

   bool init(ks_conf::Config *config) override;
};

}

#endif //QUANTITIES_H
