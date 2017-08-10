/*
 * types.h
 *
 *  Created on: 07.04.2017
 *      Author: klaus
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <iostream>
#include <functional>
#include <utility>

namespace ks {

template<class F>
struct wrapper {
   static_assert(std::is_empty<F>(), "Lambdas must be empty");

   template<class ... Ts>
   decltype(auto) operator()(Ts &&... xs) const {
      return reinterpret_cast<const F &>(*this)(std::forward<Ts>(xs)...);
   }
};

template<class F>
constexpr wrapper<F> make_function(F *) {
   return {};
}

struct wrapper_factor {
   template<class F>
   constexpr wrapper<F> operator+=(F *) {
      return {};
   }
};

struct addr_add {
   template<class T>
   friend typename std::remove_reference<T>::type *operator+(addr_add, T &&t) {
      return &t;
   }
};

#define STATIC_LAMBDA wrapper_factor() += true ? nullptr : addr_add() + []

template<bool, class T1 = void, class T2 = void>
struct IfElse {
};

template<class T1, class T2>
struct IfElse<true, T1, T2> {
   typedef T1 type;
};
template<class T1, class T2>
struct IfElse<false, T1, T2> {
   typedef T2 type;
};

}

#endif /* TYPES_H_ */
