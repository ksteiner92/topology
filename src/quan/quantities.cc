//
// Created by klaus on 09.07.17.
//

#include "quantities.h"

using namespace ks_hr;
using namespace ks_conf;
using namespace ks_sw;
using namespace ks_k;
using namespace Eigen;
using namespace std;

namespace ks {

TightBindingHamiltonianQuantity::~TightBindingHamiltonianQuantity() {
   delete value;
}

bool TightBindingHamiltonianQuantity::init(Config *config) {
   value = new TightBindingHamiltonian();
   value->setBZCartesianFactor(config->get<double>("bz", 1.0));
   if (!value->parse(config->get<string>(getName())))
      throw ConfigException("Error while parsing hr file");
   return true;
}

SelfEnergyQuantity::~SelfEnergyQuantity() {
   delete value;
}

bool SelfEnergyQuantity::init(Config *config) {
   ConfObject *swobj = config->getNodeOrNull(getName());
   if (swobj == nullptr)
      return false;
   if (swobj->getType() == OBJECT) {
      const size_t num_atoms = QuantityHandler::get()->getValue<TightBindingHamiltonian *>("hr")->getNumAtoms();
      value = new AnalyticalSelfEnergy(config->get<double>(getName() + ".w_min"),
                                       config->get<double>(getName() + ".w_max"),
                                       config->get<size_t>(getName() + ".dens"), num_atoms);
      const ConfObject *sigma = config->getNode(getName() + ".sigma");
      if (sigma->getType() == NUMBER) {
         const complex<double> sigma_const =
                 dynamic_cast<const NumberObject *>(sigma)->getValue();
         dynamic_cast<AnalyticalSelfEnergy *>(value)->setFunction(
                 [sigma_const](const double w) { return sigma_const; });
      } else {
         const string func_name =
                 dynamic_cast<const StringObject *>(sigma)->getValue();
         if (func_name == "iso") {
            const auto U = config->get<double>(getName() + ".U", 1.0);
            const auto delta = config->get<double>(getName() + ".delta", 1.0);
            dynamic_cast<AnalyticalSelfEnergy *>(value)->setFunction(
                    [U, delta](double w) { return U * U * 0.25 * (w - 1i * delta) / (w * w + delta * delta); });
         }
      }
   } else {
      value = new SelfEnergy();
      if (!dynamic_cast<SelfEnergy *>(value)->parse(config->get<string>(getName()))) {
         stringstream ss;
         ss << "Error while parsing " << getName() << " file";
         throw ConfigException(ss.str());
      }
   }
   return true;
}

RealGridQuantity::~RealGridQuantity() {
   delete value;
}

bool RealGridQuantity::init(Config *config) {
   if (config->exists(getName())) {
      vector<size_t> rgrid;
      vector<BoundaryCondition> bc;
      map<string, ks_conf::ConfObject *> grid = config->get(getName());
      function<void(const string &dim, const size_t idx)> init =
              [&](const string &dim, const size_t idx) {
                 stringstream ss1;
                 ss1 << getName() << "." << dim << ".dim";
                 stringstream ss2;
                 ss2 << getName() << "." << dim << ".bc";
                 if (rgrid.size() < idx + 1)
                    rgrid.resize(idx + 1, 1);
                 rgrid[idx] = config->get<size_t>(ss1.str());
                 const string bcstr = config->get<string>(ss2.str());
                 if (bc.size() < idx + 1)
                    bc.resize(idx + 1, BoundaryCondition::PERIODIC);
                 if (bcstr == "o")
                    bc[idx] = BoundaryCondition::OPEN;
                 else if (bcstr == "p")
                    bc[idx] = BoundaryCondition::PERIODIC;
                 else
                    cout << "Warning: Not recognized boundary condition using periodic." << bcstr
                         << endl;
              };
      for (const auto &it : grid) {
         if (it.first == "x")
            init(it.first, 0);
         else if (it.first == "y")
            init(it.first, 1);
         else if (it.first == "z")
            init(it.first, 2);
         else {
            stringstream ss;
            ss << "not recognized dimension " << it.first;
            throw ConfigException(ss.str());
         }
      }
      if (rgrid.empty()) {
         stringstream ss;
         ss << getName() << " contains no entries";
         throw ConfigException(ss.str());
      }
      value = new RealGrid(rgrid, bc);
   } else {
      stringstream ss;
      ss << getName() << " parameter does not exist in conf file";
      throw ConfigException(ss.str());
   }
   return true;
}

KPathQuantity::~KPathQuantity() {
   delete value;
}

bool KPathQuantity::init(Config *config) {
   ConfObject *kpathobj = config->getNodeOrNull("kpath");
   if (kpathobj != nullptr && kpathobj->getType() == OBJECT) {
      std::map<std::string, ConfObject *> kpathtree =
              dynamic_cast<Object *>(kpathobj)->getValue();
      if (kpathtree.size() < 4)
         throw ConfigException("kpath contains not enough entries.");
      const std::string path = config->get<std::string>("kpath.path");
      std::stringstream ss;
      ss.str(path);
      string point_name;
      vector<string> path_points;
      while (std::getline(ss, point_name, '-'))
         path_points.push_back(point_name);
      if (path_points.size() < 2)
         throw ConfigException("kpath.path contains not enough points.");
      const auto dens = config->get<size_t>("kpath.dens");
      value = new KPathBase();
      map<string, Vector3d> symmp;
      for (const auto &it : kpathtree) {
         if (it.first == "dens" || it.first == "path")
            continue;
         Vector3d symmpoint;
         symmpoint[0] = config->get<double>("kpath." + it.first + ".kx", 0.0);
         symmpoint[1] = config->get<double>("kpath." + it.first + ".ky", 0.0);
         symmpoint[2] = config->get<double>("kpath." + it.first + ".kz", 0.0);
         symmp.insert(pair<string, Vector3d>(it.first, symmpoint));
      }
      for (int i = 0; i < (path_points.size() - 1); i++) {
         const string p1_name = path_points[i];
         const string p2_name = path_points[i + 1];
         if (symmp.find(p1_name) == symmp.end()) {
            stringstream ss;
            ss << p1_name << " not specified in kpath.";
            throw ConfigException(ss.str());
         }
         if (symmp.find(p2_name) == symmp.end()) {
            stringstream ss;
            ss << p2_name << " not specified in kpath.";
            throw ConfigException(ss.str());
         }
         const Vector3d p1 = symmp[p1_name];
         const Vector3d p2 = symmp[p2_name];
         value->addSymmPoint(p1_name, p1);
         const Vector3d dk = (p2 - p1) / (double) (dens);
         for (size_t j = 1; j < dens; j++) {
            const Vector3d pos = j * dk;
            const Vector3d point = p1 + pos;
            value->addPoint(point);
         }
         if (i == (path_points.size() - 2))
            value->addSymmPoint(p2_name, p2);
      }
   } else {
      value = new KPath();
      if (!((KPath *) value)->parse(config->get<string>(getName())))
         throw ConfigException("Error while parsing kpath file");
   }
   value->setBZ(QuantityHandler::get()->getValue<TightBindingHamiltonian *>("hr")->getBZ());
   return true;
}

REGISTER_QUAN(TightBindingHamiltonianQuantity, hr);
REGISTER_QUAN(SelfEnergyQuantity, sw);
REGISTER_QUAN(SelfEnergyQuantity, sw_edge);
REGISTER_QUAN(KPathQuantity, kpath);
REGISTER_QUAN(RealGridQuantity, rgrid);
REGISTER_NUMBER_QUAN_WITH_DEFAULT(size_t, kdens, 1);
REGISTER_NUMBER_QUAN(double, idelta);

}
