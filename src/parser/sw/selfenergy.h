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

   ~SelfEnergy() override;

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
