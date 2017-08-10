/*
 * algorithm.h
 *
 *  Created on: 09.06.2017
 *      Author: klaus
 */

#ifndef ALGORITHM_ALGORITHM_H_
#define ALGORITHM_ALGORITHM_H_

namespace ks {

/**
 * IAlgorithm
 * Used as base type for Algorithm to be able
 * to store it in an container.
 */
class IAlgorithm {
};

/**
 * Algorithm interface
 *
 * Generic abstract Algorithm class from each algorithm
 * inherits.
 */
template<class Output, class ...Input> class Algorithm : public IAlgorithm {
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
