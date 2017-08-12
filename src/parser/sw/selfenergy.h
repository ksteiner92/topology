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

#ifndef SELFENERGY_H
#define SELFENERGY_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "quan/selfenergybase.h"
#include "swparser.hh"
#include "swscanner.h"

namespace ks_sw {

class SelfEnergy : public ks::SelfEnergyBase {
   friend class SWParser;

   friend class SWScanner;

public:
   SelfEnergy();

   ~SelfEnergy();

   bool parse(std::istream &in);

   bool parse(const std::string &file_name);

   const size_t getNumEnergies() const override;

   const double getEnergyAt(size_t idx) const override;

   const std::vector<std::complex<double>> &operator[](size_t idx) const
   override;

private:
   SWScanner scanner;
   SWParser parser;
   size_t location;
   bool line_finished;
   size_t num_line_atoms;
   std::vector<double> energies;
   std::vector<std::vector<std::complex<double>>> sw_w;

   void switchInputStream(std::istream *is);

   void increaseLocation(unsigned int loc);

   const size_t getLocation() const;

   void add(double value);

   const bool lineFinished();
};

}

#endif // SELFENERGY_H
