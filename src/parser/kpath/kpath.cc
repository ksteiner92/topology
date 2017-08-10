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
