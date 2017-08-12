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

#ifndef KPATH_H
#define KPATH_H

#include <iostream>
#include <fstream>
#include <vector>

#include "kparser.hh"
#include "quan/kpathbase.h"
#include "kscanner.h"
#include "Eigen/Eigenvalues"
#include "Eigen/Dense"

namespace ks_k {

class KPath : public ks::KPathBase {
   friend class KPathParser;

   friend class KPathScanner;

public:
   KPath();

   bool parse(std::istream &in);

   bool parse(const std::string &file_name);

   std::vector<size_t> getNeighborIndicesOfPoint(size_t idx) const;

   const size_t getNumNeighbors(size_t idx) const;

private:
   void switchInputStream(std::istream *is);

   void increaseLocation(unsigned int loc);

   const size_t getLocation() const;

   void addNeighbor(size_t idx, size_t neighbor);

private:
   KPathScanner scanner;
   KPathParser parser;
   size_t location;
   std::vector<std::vector<size_t>> neighbors;
};

}

#endif // KPATH_H
