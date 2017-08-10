//
// Created by klaus on 17.07.17.
//

#include "kpathbase.h"

using namespace std;
using namespace Eigen;

namespace ks {

const size_t KPathBase::getNumKPoints() const {
   return points.size();
}

void KPathBase::setBZ(const Matrix3d &bz) {
   this->bz = bz;
}

const Vector3d KPathBase::operator[](size_t idx) const {
   return points[idx].transpose() * bz;
}

const std::string KPathBase::getPointName(size_t idx) const {
   const auto it = symm_points.find(idx);
   if (it != symm_points.end())
      return it->second;
   return "";
}

void KPathBase::addPoint(const Vector3d &point) {
   points.push_back(point);
}

void KPathBase::addSymmPoint(const string &name, const Vector3d &point) {
   points.push_back(point);
   symm_points.insert(pair<size_t, string>(points.size() - 1, name));
}

}