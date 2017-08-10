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
