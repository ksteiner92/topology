%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0"
%defines
%
define parser_class_name{HamiltonParser}

%
define api
.token.constructor
%
define api
.value.
type variant
%
define parse
.assert
%
define api
.
namespace { ks_hr }
%
code requires
        {
#include <iostream>

#include <string>
#include <vector>
#include <stdint.h>
#include "Eigen/Eigenvalues"
#include "Eigen/Dense"

                using namespace std;

                namespace ks_hr {
                   class HamiltonScanner;
                   class TightBindingHamiltonian;
                }
        }

// Bison calls yylex() function that must be provided by us to suck tokens
// from the scanner. This block will be placed at the beginning of IMPLEMENTATION file (cpp).
// We define this function here (function! not method).
// This function is called only inside Bison, so we make it static to limit symbol visibility for the linker
// to avoid potential linking conflicts.
%
code top
        {
#include <iostream>
#include "hrscanner.h"

#include "hrparser.hh"
#include "tbh.h"
#include "location.hh"
#include "Eigen/Eigenvalues"
#include "Eigen/Dense"

                // yylex() arguments are defined in parser.y
                static ks_hr::HamiltonParser::symbol_type yylex(ks_hr::HamiltonScanner &scanner, ks_hr::TightBindingHamiltonian &driver) {
                   return scanner.getNextToken();
                }

                // you can accomplish the same thing by inlining the code using preprocessor
                // x and y are same as in above static function
                // #define yylex(x, y) scanner.get_next_token()

                using namespace ks_hr;
        }

%lex-param {
ks_hr::HamiltonScanner &scanner
}
%lex-param {
ks_hr::TightBindingHamiltonian &driver
}
%parse-param {
ks_hr::HamiltonScanner &scanner
}
%parse-param {
ks_hr::TightBindingHamiltonian &driver
}
%locations
%
define parse
.trace
%
define parse
.
error verbose

%
define api
.token.prefix {
TOKEN_}

%
token END
0 "end of file"
%
token <std::string> STRING
"string";
%
token<double> FLOAT
"float";
%
token<long> INT
"int"
%
token <Eigen::Vector3d> VECTOR
"vector"
%
token EOL
"end of line"

%
type <Eigen::Vector3d> vector;

%
start start

%%

start:
|
start line;

line: EOL
|
exp EOL;

exp: vector
|
hrline

        hrline:
FLOAT FLOAT
FLOAT FLOAT
FLOAT FLOAT
FLOAT {
const std::complex<double> t($6, $7);
driver.setEnergy((
int) $1, (int) $2, (int) $3, (unsigned int) $4, (unsigned int) $5, t);
};

vector: FLOAT
FLOAT FLOAT{
        $$ = Eigen::Vector3d($1, $2, $3);
        if (!driver.addLatticeVector($$))
        driver.addAtom($$);
};

%%

// Bison expects us to provide implementation - otherwise linker complains
void ks_hr::HamiltonParser::error(const location &loc, const std::string &message) {
   cout << "Error: " << message << endl << "Error location: " << driver.getLocation() << endl;
}
