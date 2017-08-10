#include "tbh.h"

using namespace Eigen;
using namespace std;

namespace ks_hr {

TightBindingHamiltonian::TightBindingHamiltonian() :
        location(0), scanner(*this), parser(scanner, *this), energy_cutoff(
        -1.0), distance_cutoff(-1.0), bzcartesianfactor(1.0) {
   parser.set_debug_level(0);
}

bool TightBindingHamiltonian::parse(const string &file_name) {
   ifstream in(file_name.c_str());
   return in.good() && parse(in);
}

bool TightBindingHamiltonian::parse(istream &in) {
   location = 0;
   switchInputStream(&in);
   return (parser.parse() == 0);
}

void TightBindingHamiltonian::setEnergyCutoff(double cutoff) {
   energy_cutoff = cutoff;
}

void TightBindingHamiltonian::setDistanceCutoff(double cutoff) {
   distance_cutoff = cutoff;
}

void TightBindingHamiltonian::setEnergy(int x, int y, int z, unsigned int n, unsigned int m, complex<double> t) {
   if ((t.real() * t.real() + t.imag() * t.imag())
       > (energy_cutoff * energy_cutoff)) {
      //TODO handle energy cutoff
   }
   const tuple<int, int, int> neighbor = make_tuple(x, y, z);
   neighbors.insert(neighbor);
   MatrixXcd matrix;
   const auto it = T_ij.find(neighbor);
   if (it == T_ij.end()) {
      const size_t dim = atoms.size();
      matrix = MatrixXcd::Zero(dim, dim);
      T_ij[neighbor] = matrix;
   } else
      matrix = it->second;
   matrix(n - 1, m - 1) = t;
   T_ij[neighbor] = matrix;
   Vector3d r_cell = lattice_vecs[0] * x;
   r_cell += lattice_vecs[1] * y;
   r_cell += lattice_vecs[2] * z;
   Vector3d r_atom = r_cell + atoms[m - 1];
   r_atom -= atoms[n - 1];
   if (r_atom.norm() > distance_cutoff) {
      //TODO handle distance cutoff
   }
   cell_pos[neighbor] = r_cell;
   //if (H.size() <= 0)
   //    H = MatrixXcd::Zero(dim, dim);
   //H(n, m) +=
}

void TightBindingHamiltonian::setBZCartesianFactor(double bzcartesianfactor) {
   this->bzcartesianfactor = bzcartesianfactor;
}

void TightBindingHamiltonian::switchInputStream(istream *is) {
   scanner.switch_streams(is, nullptr);
}

void TightBindingHamiltonian::increaseLocation(unsigned int loc) {
   location += loc;
}

const size_t TightBindingHamiltonian::getLocation() const {
   return location;
}

const bool inline TightBindingHamiltonian::unitCellSet() const {
   return lattice_vecs.size() >= 3;
}

const bool TightBindingHamiltonian::addLatticeVector(const Vector3d &v) {
   if (!unitCellSet()) {
      lattice_vecs.push_back(v);
      const size_t currentIdx = lattice_vecs.size() - 1;
      unit_cell.col(currentIdx) << lattice_vecs[currentIdx];
      if (bzcartesianfactor > 1e-14) {
         bzone = Matrix3d::Zero();
         for (size_t i = 0; i < 3; i++)
            bzone(i, i) = 2.0 * M_PI
                          / (bzcartesianfactor * lattice_vecs[i].norm());
      } else {
         bzone = unit_cell;
         bzone = (2.0 * M_PI * bzone.inverse());
      }
      return true;
   }
   return false;
}

void TightBindingHamiltonian::addAtom(const Vector3d &atom) {
   atoms.push_back(atom);
}

Matrix3d TightBindingHamiltonian::getUnitCell() const {
   return unit_cell;
}

const Matrix3d &TightBindingHamiltonian::getBZ() const {
   return bzone;
}

const size_t TightBindingHamiltonian::getNumAtoms() const {
   return atoms.size();
}

const Vector3d &TightBindingHamiltonian::getPosOfNeighbor(NeighborType neighbor) {
   return cell_pos[neighbor];
}

const map<ks_hr::TightBindingHamiltonian::NeighborType, MatrixXcd> &TightBindingHamiltonian::getTij() const {
   return T_ij;
}

void TightBindingHamiltonian::getLatticeVectors(vector<Vector3d> &lattice_vecs) const {
   lattice_vecs = this->lattice_vecs;
}

void TightBindingHamiltonian::getAtoms(vector<Vector3d> &atoms) const {
   atoms = this->atoms;
}

void TightBindingHamiltonian::getNeighbors(vector<NeighborType> &neighbors) const {
   copy(this->neighbors.begin(), this->neighbors.end(), neighbors.begin());
}

void TightBindingHamiltonian::getNeighborPositions(
        map<NeighborType, Vector3d> &neighbor_pos) const {

}

}
