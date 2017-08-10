#include "selfenergy.h"

using namespace std;
using namespace Eigen;
using namespace ks;

namespace ks_sw {

SelfEnergy::SelfEnergy() :
        location(0), scanner(*this), parser(scanner, *this), line_finished(
        true), num_line_atoms(0) {
   parser.set_debug_level(0);
}

SelfEnergy::~SelfEnergy() = default;

bool SelfEnergy::parse(const string &file_name) {
   ifstream in(file_name.c_str());
   return in.good() && parse(in);
}

bool SelfEnergy::parse(istream &in) {
   location = 0;
   switchInputStream(&in);
   return (parser.parse() == 0);
}

const size_t SelfEnergy::getNumEnergies() const {
   return energies.size();
}

const double SelfEnergy::getEnergyAt(size_t idx) const {
   return energies[idx];
}

const vector<complex<double>> &SelfEnergy::operator[](size_t idx) const {
   return sw_w[idx];
}

void SelfEnergy::switchInputStream(istream *is) {
   scanner.switch_streams(is, nullptr);
}

void SelfEnergy::increaseLocation(unsigned int loc) {
   location += loc;
}

const size_t SelfEnergy::getLocation() const {
   return location;
}

void SelfEnergy::add(double value) {
   if (line_finished) {
      energies.push_back(value);
      vector<complex<double>> sw_p_omega;
      sw_w.push_back(sw_p_omega);
      line_finished = false;
   } else {
      num_line_atoms++;
      if ((num_line_atoms % 2) == 0) {
         sw_w.back().back().imag(value);
      } else {
         complex<double> sw(value, 0.0);
         sw_w.back().push_back(sw);
      }
   }
}

const bool SelfEnergy::lineFinished() {
   line_finished = true;
   const bool valid = (num_line_atoms % 2 == 0);
   num_line_atoms = 0;
   return valid;
}

}
