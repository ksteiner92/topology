/**
 * This file is part of toplogy.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * algorithm.h
 *
 * Contains the interface class for algorithms
 *
 *  Created on: 09.06.2017
 *      Author: Klaus Steiner
 */

#ifndef ALGORITHM_ALGORITHM_H_
#define ALGORITHM_ALGORITHM_H_

namespace ks {

/**
 * Algorithm interface
 *
 * Generic abstract Algorithm class from each algorithm
 * inherits.
 */
template<class Output, class ...Input> class Algorithm {
public:
   /**
    * Pure virtual operator. Must be implemented and
    * contains the algorithm.
    *
    * @param out The output of the algorithm
    * @param in The inputs for the alogirthm.
    */
   virtual void operator()(Output &out, const Input &...in) = 0;
};

}

#endif /* ALGORITHM_ALGORITHM_H_ */
