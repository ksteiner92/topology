#ifndef PARSER_HR_TBH_H
#define PARSER_HR_TBH_H

#include <iostream>
#include <vector>
#include <map>
#include <tuple>
#include <set>
#include <sstream>
#include <fstream>

#include "hrparser.hh"
#include "Eigen/Eigenvalues"
#include "Eigen/Dense"
#include "hrscanner.h"

namespace ks_hr {

class TightBindingHamiltonian {
   friend class HamiltonParser;

   friend class HamiltonScanner;

public:
   typedef std::tuple<int, int, int> NeighborType;

   TightBindingHamiltonian();

   bool parse(std::istream &in);

   bool parse(const std::string &file_name);

   Eigen::Matrix3d getUnitCell() const;

   const Eigen::Matrix3d &getBZ() const;

   void setEnergyCutoff(double cutoff);

   void setDistanceCutoff(double cutoff);

   void setBZCartesianFactor(double bzcartesianfactor);

   void getAtoms(std::vector<Eigen::Vector3d> &atoms) const;

   void getLatticeVectors(std::vector<Eigen::Vector3d> &lattice_vecs) const;

   void getNeighbors(std::vector<NeighborType> &neighbors) const;

   void getNeighborPositions(
           std::map<NeighborType, Eigen::Vector3d> &neighbor_pos) const;

   const Eigen::Vector3d &getPosOfNeighbor(NeighborType neighbor);

   const size_t getNumAtoms() const;

   const std::map<NeighborType, Eigen::MatrixXcd> &getTij() const;

private:
   void switchInputStream(std::istream *is);

   const bool inline unitCellSet() const;

   const bool addLatticeVector(const Eigen::Vector3d &v);

   void addAtom(const Eigen::Vector3d &atom);

   void setEnergy(int x, int y, int z, unsigned int n, unsigned int m, std::complex<double> t);

   void increaseLocation(unsigned int loc);

   const size_t getLocation() const;

private:
   HamiltonScanner scanner;
   HamiltonParser parser;
   Eigen::Matrix3d unit_cell;
   Eigen::Matrix3d bzone;
   double bzcartesianfactor;
   size_t location;
   double energy_cutoff;
   double distance_cutoff;
   std::vector<Eigen::Vector3d> lattice_vecs;
   std::vector<Eigen::Vector3d> atoms;
   std::set<NeighborType> neighbors;
   std::map<NeighborType, Eigen::Vector3d> cell_pos;
   std::map<NeighborType, Eigen::MatrixXcd> T_ij;
};

}

#endif // PARSER_HR_TBH_H
