/*
 * confobj.h
 *
 *  Created on: 08.05.2017
 *      Author: klaus
 */

#ifndef PARSER_CONFIG_CONFOBJ_H_
#define PARSER_CONFIG_CONFOBJ_H_

#include <iostream>
#include <complex>

namespace ks_conf {

enum ObjectType {
   STRING, NUMBER, OBJECT, FUNCTION
};

class ConfObject {
public:
   virtual ~ConfObject() {
   }

   virtual const ObjectType getType() const = 0;
};

class NumberObject : public ConfObject {
public:
   NumberObject() :
           value(0.0) {
   }

   virtual ~NumberObject() {
   }

   virtual const ObjectType getType() const override {
      return NUMBER;
   }

   std::complex<double> getValue() const {
      return value;
   }

   NumberObject *setValue(const std::complex<double> &value) {
      this->value = value;
      return this;
   }

private:
   std::complex<double> value;
};

class StringObject : public ConfObject {
public:
   StringObject() {
   }

   StringObject(const std::string &value) {
      this->value = value;
   }

   virtual ~StringObject() {
   }

   virtual const ObjectType getType() const override {
      return STRING;
   }

   const std::string &getValue() const {
      return value;
   }

   StringObject *setValue(const std::string &value) {
      this->value = value;
      return this;
   }

private:
   std::string value;
};

class Object : public ConfObject {
public:
   virtual ~Object() {
   }

   virtual const ObjectType getType() const override {
      return OBJECT;
   }

   const std::map<std::string, ConfObject *> &getValue() const {
      return value;
   }

   Object *setValue(const std::map<std::string, ConfObject *> &value) {
      this->value = value;
      return this;
   }

   const size_t getNumNodes() const {
      return value.size();
   }

private:
   std::map<std::string, ConfObject *> value;
};

class BaseExpression {
public:
   virtual ~BaseExpression() {
   }

   virtual const std::complex<double> operator()(const std::complex<double> &a,
                                                 const std::complex<double> &b) = 0;

   /* virtual const std::complex<double> operator()(const BaseExpression &a,
                const std::complex<double> &b) = 0;

    virtual const std::complex<double> operator()(const BaseExpression &a,
                    const BaseExpression &b) = 0;

    virtual const std::complex<double> operator()(const std::complex<double> &a,
                const BaseExpression &b) = 0;*/
};

class Addition : public BaseExpression {
public:
   virtual ~Addition() {
   }

   const std::complex<double> operator()(const std::complex<double> &a,
                                         const std::complex<double> &b) {
      return a + b;
   }
};

class Subtraction : public BaseExpression {
public:
   virtual ~Subtraction() {
   }

   const std::complex<double> operator()(const std::complex<double> &a,
                                         const std::complex<double> &b) {
      return a - b;
   }
};

class Multiplication : public BaseExpression {
public:
   virtual ~Multiplication() {
   }

   const std::complex<double> operator()(const std::complex<double> &a,
                                         const std::complex<double> &b) {
      return a * b;
   }
};

class Division : public BaseExpression {
public:
   virtual ~Division() {
   }

   const std::complex<double> operator()(const std::complex<double> &a,
                                         const std::complex<double> &b) {
      return a / b;
   }
};

class Power : public BaseExpression {
public:
   virtual ~Power() {
   }

   const std::complex<double> operator()(const std::complex<double> &a,
                                         const std::complex<double> &b) {
      return pow(a, b);
   }
};

class FunctionObject : public ConfObject {
public:
   virtual ~FunctionObject() {
   }

   virtual const ObjectType getType() const override {
      return FUNCTION;
   }
};

}

#endif /* PARSER_CONFIG_CONFOBJ_H_ */
