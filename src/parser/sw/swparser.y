%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0"
%defines
%define parser_class_name { SWParser }
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define api.namespace { ks_sw }
%code requires
{
    #include <iostream>
    #include <string>
    #include <vector>
    #include <stdint.h>
    #include "Eigen/Eigenvalues"
	#include "Eigen/Dense"

    using namespace std;

    namespace ks_sw {
        class SWScanner;
        class SelfEnergy;
    }
}

// Bison calls yylex() function that must be provided by us to suck tokens
// from the scanner. This block will be placed at the beginning of IMPLEMENTATION file (cpp).
// We define this function here (function! not method).
// This function is called only inside Bison, so we make it static to limit symbol visibility for the linker
// to avoid potential linking conflicts.
%code top
{
    #include <iostream>
    #include <complex>
    #include "swscanner.h"
    #include "swparser.hh"
    #include "selfenergy.h"
    #include "location.hh"
    #include "Eigen/Eigenvalues"
	#include "Eigen/Dense"
    
    // yylex() arguments are defined in parser.y
    static ks_sw::SWParser::symbol_type yylex(ks_sw::SWScanner &scanner, ks_sw::SelfEnergy &driver) {
        return scanner.getNextToken();
    }
    
    // you can accomplish the same thing by inlining the code using preprocessor
    // x and y are same as in above static function
    // #define yylex(x, y) scanner.get_next_token()
    
    using namespace ks_sw;
}

%lex-param { ks_sw::SWScanner &scanner }
%lex-param { ks_sw::SelfEnergy &driver }
%parse-param { ks_sw::SWScanner &scanner }
%parse-param { ks_sw::SelfEnergy &driver }
%locations
%define parse.trace
%define parse.error verbose

%define api.token.prefix {TOKEN_}

%token END 0 "end of file"
%token <double> NUMBER "number";
%token EOL "end of line"

%start start

%%

start:
     | start value
     | start EOL {
     	driver.lineFinished();
     }
     | END
;
   
value: NUMBER {
    driver.add($1);
}
;
     
%%

// Bison expects us to provide implementation - otherwise linker complains
void ks_sw::SWParser::error(const location &loc , const std::string &message) {
	cout << "Error: " << message << endl << "Error location: " << driver.getLocation() << endl;
}
