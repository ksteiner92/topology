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

#include "kpath.h"

using namespace std;
using namespace Eigen;

namespace ks_k {

KPath::KPath() :
        location(0), scanner(*this), parser(scanner, *this) {
   parser.set_debug_level(0);
   bz = Matrix3d::Identity();
}

bool KPath::parse(const string &file_name) {
   ifstream in(file_name.c_str());
   return in.good() && parse(in);
}

bool KPath::parse(istream &in) {
   location = 0;
   switchInputStream(&in);
   return (parser.parse() == 0);
}

void KPath::switchInputStream(istream *is) {
   scanner.switch_streams(is, nullptr);
}

void KPath::increaseLocation(unsigned int loc) {
   location += loc;
}

const size_t KPath::getLocation() const {
   return location;
}

std::vector<size_t> KPath::getNeighborIndicesOfPoint(size_t idx) const {
   return neighbors[idx];
}

const size_t KPath::getNumNeighbors(size_t idx) const {
   return neighbors[idx].size();
}

void KPath::addNeighbor(size_t idx, size_t neighbor) {
   if (neighbors.size() <= idx)
      neighbors.resize(idx + 1);
   neighbors[idx].push_back(neighbor);
}

}
