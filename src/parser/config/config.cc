#include "config.h"

using namespace std;

namespace ks_conf {

Config::Config() :
        location(0), scanner(*this), parser(scanner, *this) {
   parser.set_debug_level(0);
}

Config::~Config() {
   for (auto it : tree)
      deleteConfObject(it.second);
}

void Config::deleteConfObject(ConfObject *obj) {
   if (obj->getType() == ObjectType::OBJECT) {
      auto *node = dynamic_cast<Object *>(obj);
      for (auto it : node->getValue())
         deleteConfObject(it.second);
   }
   delete obj;
}

bool Config::parse(const string &file_name) {
   ifstream in(file_name.c_str());
   if (!in.good())
      return false;
   if (!parse(in))
      return false;
   StringObject *hr = dynamic_cast<StringObject *>(tree["hr"]);
   if (hr == nullptr) {
      cout << "hr parameter missing" << endl;
      return false;
   }
   /*ks_hr::TightBinding tb;
    tb.setBZCartesianFactor(0.0);
    if (!tb.parse(hr->getValue())) {
    cerr << "Error while parsing hr file" << endl;
    return 1;
    }*/
   return true;
}

bool Config::parse(istream &in) {
   location = 0;
   switchInputStream(&in);
   return (parser.parse() == 0);
}

std::map<std::string, ConfObject *> &Config::getTree() {
   return tree;
}

void Config::switchInputStream(istream *is) {
   scanner.switch_streams(is, nullptr);
}

void Config::increaseLocation(unsigned int loc) {
   location += loc;
}

const size_t Config::getLocation() const {
   return location;
}

void Config::insert(const std::pair<std::string, ConfObject *> &item) {
   tree.insert(item);
}

}
