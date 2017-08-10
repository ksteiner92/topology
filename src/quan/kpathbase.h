/*
 * kpath.h
 *
 *  Created on: 18.06.2017
 *      Author: klaus
 */

#ifndef KPATHBASE_H_
#define KPATHBASE_H_

#include <string>
#include <vector>
#include <map>

#include "Eigen/Eigenvalues"
#include "Eigen/Dense"

namespace ks {

class KPathBase {
public:
   const size_t getNumKPoints() const;

   const Eigen::Vector3d operator[](size_t idx) const;

   const std::string getPointName(size_t idx) const;

   void setBZ(const Eigen::Matrix3d &bz);

   void addPoint(const Eigen::Vector3d &point);

   void addSymmPoint(const std::string &name, const Eigen::Vector3d &point);

protected:
   Eigen::Matrix3d bz;
   std::vector<Eigen::Vector3d> points;
   std::map<size_t, std::string> symm_points;
};

}

#endif /* KPATHBASE_H_ */
